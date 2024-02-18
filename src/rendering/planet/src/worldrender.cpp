
#include "worldrender.h"

#include <vector>
#include "shader.h"
#include "camera.h"
#include "image.h"
#include "quadsphere.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <stb_image.h>

#include "wgs84.hpp"
#include "geometry.h"

#include "plane.h"

#include "wms_client.hpp"


// ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace geodecy;

namespace render
{

WorldRenderer::WorldRenderer()
{

}

bool WorldRenderer::init()
{
	if(!glfwInit())
	{
		std::printf("Failed to init GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_windowWidth = 800;
	m_windowHeight = 600;
	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "World Simulation", NULL, NULL);
	if(m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwSetWindowUserPointer(m_window, this);
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
	{
		WorldRenderer* renderer = static_cast<WorldRenderer*>(glfwGetWindowUserPointer(window));

		glViewport(0, 0, width, height);
		renderer->m_windowWidth = width;
		renderer->m_windowHeight = height;
	});

	glfwMakeContextCurrent(m_window);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

// TODO: Remove
struct FrameData
{
	camera::Camera renderCamera;
	camera::Camera lodCamera;
	plane::Plane testPlane;

	bool showWireframe{false};
	int speedZoom{18};
	int cameraMode{0};

	int earthTextureId;
	int earthDepthmapId;

	quadsphere::data quadsphere;
	geometry::test_geometry geometry_test_data;
} frameData;


void WorldRenderer::renderFrame()
{
	// TODO: TEMPORARY FIX
	auto& renderCamera = frameData.renderCamera;
	auto& lodCamera = frameData.lodCamera; (void) lodCamera;
	auto& showWireframe = frameData.showWireframe;
	auto& speedZoom = frameData.speedZoom;
	auto& cameraMode = frameData.cameraMode;

	auto& quadsphere = frameData.quadsphere;
	auto& geometry_test_data = frameData.geometry_test_data;

	auto& testPlane = frameData.testPlane;

	auto& earthTextureId = frameData.earthTextureId;
	auto& earthDepthmapId = frameData.earthDepthmapId;

	// Setup clear color and clear buffers
	glClearColor(0.3, 0.3, 0.3, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: Setup input handler
	int yawDir = 0; (void) yawDir;
	int pitchDir = 0; (void) pitchDir;
	int zz = 0;
	int xx = 0;
	int yy = 0; (void) yy;
	{
		constexpr double rotationSpeed = 2;
		if(glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) renderCamera.rotation.y -= rotationSpeed;
		if(glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS) renderCamera.rotation.y += rotationSpeed;
		if(glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) { yawDir++; renderCamera.rotation.z += rotationSpeed; }
		if(glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) { yawDir--; renderCamera.rotation.z -= rotationSpeed; }
		if(glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) { pitchDir--; }
		if(glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS) { pitchDir++; }

		if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) xx++;
		if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) xx--;
		if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) yy++;
		if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) yy--;
		if(glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) zz++;
		if(glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) zz--;
	}

	double speed_mps = std::pow(2, speedZoom) / 60.0; (void) speed_mps;
	if(!std::isfinite(speed_mps))
	{
		speed_mps = 0;
	}

	{
		glm::dvec3 forwardVector = testPlane.getForwardVector();
		glm::dvec3 newPosition =
			testPlane.position
			+ (forwardVector * glm::dvec3(xx * speed_mps));

		// Calculate fly height
		double flyHeight = testPlane.getAltitude() + (zz * speed_mps / 2.0);
		flyHeight = std::max(-1'000'000.0, flyHeight);

		// Calculate gravity up and clamp position
		glm::dvec3 gravityUp;
		{
			auto nwu = testPlane.getLocalTangentPlane();
			auto lla = wgs84::deg::xyz2lla(newPosition);

			gravityUp = nwu * glm::dvec3(0, 0, 1);
			newPosition = wgs84::deg::lla2xyz(lla.x, lla.y, flyHeight);
		}

		testPlane.position = newPosition;

		// Rotate around
		testPlane.rotation = glm::quat_cast(
			glm::rotate(glm::dmat4(1.0), glm::radians(yawDir * 2 + 0.0), gravityUp)
			* glm::rotate(glm::dmat4(1.0), glm::radians(pitchDir * 2 + 0.0), testPlane.rotation * glm::dvec3(0, 1, 0))
			* glm::mat4_cast(testPlane.rotation)
		);

		// Apply curvature fix
		{
			auto n_lla = wgs84::deg::xyz2lla(newPosition);
			auto n_nwu = wgs84::deg::lla2nwu(n_lla.x, n_lla.y);
			auto n_up = n_nwu * glm::dvec3(0, 0, 1);

			// Rotation needed to fix rotation
			glm::dvec3 axis = glm::normalize(glm::cross(gravityUp, n_up));
			if(!glm::any(glm::isnan(axis)))
			{
				testPlane.rotation = glm::angleAxis(glm::angle(gravityUp, n_up), axis) * testPlane.rotation;
			}
		}

		// Calculate planes yaw pitch roll
		{
			const auto nwu = testPlane.getLocalTangentPlane();
			const auto rpy = testPlane.getLTP_RPY_rad();
			testPlane.rotation = nwu * geodecy::rpy_rad(rpy.x, rpy.y, rpy.z);
		}

		if(cameraMode == 0) // Top down
		{
			auto lla = wgs84::deg::xyz2lla(testPlane.position);
			renderCamera.position = wgs84::deg::lla2xyz(lla.x, lla.y, lla.z + 3'000'000);
			renderCamera.rotation = testPlane.getLocalTangentPlane()
				* glm::dmat3(glm::angleAxis(glm::radians(90.0), glm::dvec3(0, 1, 0)));
		}
		else if(cameraMode == 1) // FOV
		{
			renderCamera.rotation = testPlane.rotation;
			renderCamera.position = testPlane.position;
		}
	}

	glm::mat4 viewMatrix = glm::translate(glm::dmat4(renderCamera.getPerspectiveMatrix(
		m_windowWidth,
		m_windowHeight
	) * glm::inverse(renderCamera.getCameraMatrix())
	), glm::dvec3(-renderCamera.position) * glm::dvec3(1.0 / wgs84::c_A));

	{
		// Draw plane
		glUseProgram(geometry_test_data.programId);
		glm::dmat4 lodViewMatrixDouble = glm::translate(
			glm::dmat4(viewMatrix),
			glm::dvec3(testPlane.position) * glm::dvec3(1.0 / wgs84::c_A, 1.0 / wgs84::c_A, 1.0 / wgs84::c_B));
		lodViewMatrixDouble = glm::scale(lodViewMatrixDouble, glm::dvec3(0.2));

		lodViewMatrixDouble = lodViewMatrixDouble * glm::dmat4(testPlane.rotation);

		glm::mat4 lodViewMatrix = lodViewMatrixDouble;
		glUniformMatrix4fv(geometry_test_data.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(lodViewMatrix));
		if(cameraMode != 1)
		{
			geometry_test_data.xyz_arrow.draw();
		}
	}

	geometry::test_draw(
		geometry_test_data,
		glm::identity<glm::mat4>(),
		viewMatrix,
		m_windowWidth,
		m_windowHeight
	);

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	{
		camera::Camera& lodCam = renderCamera; (void) lodCam;// lodCamera
		/*
		{
			glUseProgram(geometry_test_data.programId);
			glm::dmat4 lodViewMatrixDouble = glm::translate(
				glm::dmat4(viewMatrix),
				glm::dvec3(lodCam.position) * glm::dvec3(1.0 / wgs84::c_A));
			lodViewMatrixDouble = glm::scale(lodViewMatrixDouble, glm::dvec3(0.1));

			auto lla_deg = wgs84::ecef2lla(lodCam.position);
			auto nuw_view = wgs84::lla2nwu(lla_deg.x, lla_deg.y); (void) nuw_view;
			lodViewMatrixDouble = lodViewMatrixDouble * glm::dmat4(nuw_view);

			glm::mat4 lodViewMatrix = lodViewMatrixDouble;
			glUniformMatrix4fv(geometry_test_data.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(lodViewMatrix));
			geometry_test_data.xyz_arrow.draw();
		}
		*/

		if(showWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glUseProgram(quadsphere.shaderProgramId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthTextureId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, earthDepthmapId);
		glm::mat4 test_matrix = glm::scale(
			viewMatrix, glm::vec3(1, 1, wgs84::c_B / wgs84::c_A)
		);
		glUniformMatrix4fv(quadsphere.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(test_matrix)); // viewMatrix));
		quadsphere::render(
			quadsphere,
			lodCam.position * glm::dvec3(1.0 / wgs84::c_A));
		glUseProgram(0);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void WorldRenderer::renderGui()
{
	auto& io = ImGui::GetIO();
	ImGui::Begin("Information", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos({0, 0});
	ImGui::SetWindowSize({300, 250});

	ImGui::Checkbox("Show Wireframe", &frameData.showWireframe);
	ImGui::InputInt("SpeedZoom", &frameData.speedZoom);
	ImGui::InputInt("CameraMode", &frameData.cameraMode);
	frameData.cameraMode = ((frameData.cameraMode % 3) + 3) % 3;
	ImGui::Separator();

	auto lla = frameData.testPlane.getLLA();
	ImGui::Text("Latitude:  %.3f deg", lla.x);
	ImGui::Text("Longitude: %.3f deg", lla.y);
	ImGui::Text("Altitude:  %.3f meter", lla.z);
	ImGui::Separator();

	auto rpy = frameData.testPlane.getLTP_RPY_deg();
	ImGui::Text("Roll:  %.3f deg", rpy.x);
	ImGui::Text("Pitch: %.3f deg", rpy.y);
	ImGui::Text("Yaw:   %.3f deg", rpy.z);
	ImGui::Separator();

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();

	ImGui::ShowDemoWindow();
}

void WorldRenderer::run()
{
	std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));

	frameData.quadsphere = quadsphere::generateQuadsphereData(4);
	frameData.geometry_test_data = geometry::test();

	frameData.earthTextureId = image::loadImage("assets/earth_texture_10k.jpg");
	frameData.earthDepthmapId = image::loadImage("assets/earth_depthmap_10k.jpg");
	std::printf("Read texture %u\n", frameData.earthTextureId);
	std::printf("Read texture %u\n", frameData.earthDepthmapId);
	
	{
		wms::WMSClient wmsClient;

		std::cout << "======================================" <<  std::endl;
		const wms::WMSCapabilities* capabilities = wmsClient.openCapabilities(
			"https://gibs.earthdata.nasa.gov/wms/epsg4326/best/wms.cgi?SERVICE=WMS&REQUEST=GetCapabilities&VERSION=1.3.0");
		(void) capabilities;

		std::vector<unsigned char> buffer = wmsClient.getImage(capabilities, "BlueMarble_NextGeneration", -90, 90, -180, 180, 4096, 2048);
		frameData.earthTextureId = image::loadImageFromBuffer(buffer.data(), buffer.size());
	}

	glUseProgram(frameData.quadsphere.shaderProgramId);
	glUniform1i(glGetUniformLocation(frameData.quadsphere.shaderProgramId, "earthTexture"), 0);
	glUniform1i(glGetUniformLocation(frameData.quadsphere.shaderProgramId, "earthDepthmap"), 1);
	glUseProgram(0);

	frameData.renderCamera.position = wgs84::deg::lla2xyz(0, 0, 3'000'000);
	frameData.renderCamera.rotation = glm::quat_cast(wgs84::deg::lla2nwu(90, 0));

	frameData.testPlane.position = wgs84::deg::lla2xyz(0, 0, 1000);
	frameData.testPlane.rotation = glm::quat_cast(wgs84::deg::lla2nwu(0, 0))
		* glm::angleAxis(glm::radians(-90.0), glm::dvec3(0, 0, 1))
		* glm::angleAxis(glm::radians(-45.0), glm::dvec3(0, 1, 0));

	// {
	//	 double lat = 27.986065;
	//	 double lon = 86.922623;
	//	 frameData.testPlane.position = wgs84::deg::lla2xyz(lat, lon, 1000);
	//	 frameData.testPlane.rotation = glm::quat_cast(wgs84::deg::lla2nwu(lat, lon));
	// }

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	using namespace std::chrono;
	int frames = 0;
	auto fpsDuration = nanoseconds(static_cast<uint64_t>(1'000'000'000 / m_targetFps));
	auto nextSecond = high_resolution_clock::now();
	auto nextFrame = high_resolution_clock::now();
	while(!glfwWindowShouldClose(m_window))
	{
		// Poll window events
		glfwPollEvents();

		std::this_thread::sleep_until(nextFrame);
		nextFrame += fpsDuration;

		frames ++;
		if(high_resolution_clock::now() > nextSecond)
		{
			std::printf("Frames %d\n", frames);
			frames = 0;
			nextSecond += seconds(1);
		}

		// Render frame
		renderFrame();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		renderGui();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_window);
	}

	// Shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Shutdown glfw
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

} // render
