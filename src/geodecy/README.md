# Geodecy
Contains code to calculate these for any given spheroid:
- (LTP) local tangent plane
  - Any derivitive of the ltp can also be generated
    using the Axis enum to customize axis
- (LLA) latitude, longitude, altitude
- (XYZ) xyz cartesian coordinates

The geodecy code currently supports:
- wgs84 (with glm)

## Allocator structure
The allocator needs to contain certain defines and methods to create any output

Here is an example allocator for the type `std::tuple`
```cpp
struct spheroid_tuple_allocator
{
    /// (required) The type of the result values
    using Type = double;

    /// (required) The vector type
    using Vec3 = std::tuple<Type, Type, Type>;

    /// (required) The matrix type
    using Mat3 = std::tuple<
        Type, Type, Type,
        Type, Type, Type,
        Type, Type, Type>;

    /// (required) Convertion from 9 values to a matrix
    static constexpr Mat3 to_mat(
        Type a_x0, Type a_y0, Type a_z0,
        Type a_x1, Type a_y1, Type a_z1,
        Type a_x2, Type a_y2, Type a_z2)
    {
        return {
            a_x0, a_y0, a_z0,
            a_x1, a_y1, a_z1,
            a_x2, a_y2, a_z2};
    }

    /// (required) Convertion from 3 values to a vector
    static constexpr Vec3 to_vec(
        Type a_x, Type a_y, Type a_z)
    {
        return {a_x, a_y, a_z};
    }

    /// (required) Getter for vector x component
    static constexpr Type get_x(const Vec3& a_vec)
    {
        return std::get<0>(a_vec);
    }

    /// (required) Getter for vector y component
    static constexpr Type get_y(const Vec3& a_vec)
    {
        return std::get<1>(a_vec);
    }

    /// (required) Getter for vector z component
    static constexpr Type get_z(const Vec3& a_vec)
    {
        return std::get<2>(a_vec);
    }
};
```