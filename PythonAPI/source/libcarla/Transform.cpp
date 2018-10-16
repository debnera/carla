// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <carla/geom/Transform.h>

#include <ostream>

namespace carla {
namespace geom {

  std::ostream &operator<<(std::ostream &out, const Vector3D &vector3D) {
    out << "Vector3D(x=" << vector3D.x
        << ", y=" << vector3D.y
        << ", z=" << vector3D.z << ')';
    return out;
  }

  std::ostream &operator<<(std::ostream &out, const Location &location) {
    out << "Location(x=" << location.x
        << ", y=" << location.y
        << ", z=" << location.z << ')';
    return out;
  }

  std::ostream &operator<<(std::ostream &out, const Rotation &rotation) {
    out << "Rotation(pitch=" << rotation.pitch
        << ", yaw=" << rotation.yaw
        << ", roll=" << rotation.roll << ')';
    return out;
  }

  std::ostream &operator<<(std::ostream &out, const Transform &transform) {
    out << "Transform(" << transform.location << ", " << transform.rotation << ')';
    return out;
  }

} // namespace geom
} // namespace carla

void export_transform() {
  using namespace boost::python;
  namespace cg = carla::geom;

  class_<cg::Vector3D>("Vector3D")
    .def(init<float, float, float>((arg("x")=0.0f, arg("y")=0.0f, arg("z")=0.0f)))
    .def_readwrite("x", &cg::Vector3D::x)
    .def_readwrite("y", &cg::Vector3D::y)
    .def_readwrite("z", &cg::Vector3D::z)
    .def("__eq__", &cg::Vector3D::operator==)
    .def("__ne__", &cg::Vector3D::operator!=)
    .def(self += self)
    .def(self + self)
    .def(self -= self)
    .def(self - self)
    .def(self_ns::str(self_ns::self))
  ;

  class_<cg::Location>("Location")
    .def(init<float, float, float>((arg("x")=0.0f, arg("y")=0.0f, arg("z")=0.0f)))
    .add_property("x", +[](const cg::Location &self) { return self.x; }, +[](cg::Location &self, float x) { self.x = x; })
    .add_property("y", +[](const cg::Location &self) { return self.y; }, +[](cg::Location &self, float y) { self.y = y; })
    .add_property("z", +[](const cg::Location &self) { return self.z; }, +[](cg::Location &self, float z) { self.z = z; })
    .def("__eq__", &cg::Location::operator==)
    .def("__ne__", &cg::Location::operator!=)
    .def(self += self)
    .def(self + self)
    .def(self -= self)
    .def(self - self)
    .def(self_ns::str(self_ns::self))
  ;

  class_<cg::Rotation>("Rotation")
    .def(init<float, float, float>((arg("pitch")=0.0f, arg("yaw")=0.0f, arg("roll")=0.0f)))
    .def_readwrite("pitch", &cg::Rotation::pitch)
    .def_readwrite("yaw", &cg::Rotation::yaw)
    .def_readwrite("roll", &cg::Rotation::roll)
    .def("__eq__", &cg::Rotation::operator==)
    .def("__ne__", &cg::Rotation::operator!=)
    .def(self_ns::str(self_ns::self))
  ;

  class_<cg::Transform>("Transform")
    .def(init<cg::Location, cg::Rotation>(
        (arg("location")=cg::Location(), arg("rotation")=cg::Rotation())))
    .def_readwrite("location", &cg::Transform::location)
    .def_readwrite("rotation", &cg::Transform::rotation)
    .def("__eq__", &cg::Transform::operator==)
    .def("__ne__", &cg::Transform::operator!=)
    .def(self_ns::str(self_ns::self))
  ;
}
