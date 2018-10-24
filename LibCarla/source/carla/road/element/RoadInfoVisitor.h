// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/road/element/RoadSegment.h"
#include "carla/road/element/RoadInfo.h"

#include <iterator>

namespace carla {
namespace road {
namespace element {

  class RoadInfo;
  class RoadInfoLane;
  class RoadInfoVelocity;

  class RoadInfoVisitor {
  public:

    virtual void Visit(RoadInfoLane &) {}
    virtual void Visit(RoadInfoVelocity &) {}
  };

  template <typename T, typename IT>
  class RoadInfoIterator : private RoadInfoVisitor {
  public:

    static_assert(std::is_same<std::shared_ptr<RoadInfo>, typename IT::value_type>::value, "Not compatible.");

    RoadInfoIterator(IT begin, IT end)
      : _it(begin),
        _end(end) {
      _success = false;
      for (; !IsAtEnd(); ++_it) {
        (*_it)->AcceptVisitor(*this);
        if (_success) {
          break;
        }
      }
    }

    RoadInfoIterator &operator++() {
      _success = false;
      while (!_success) {
        ++_it;
        if (IsAtEnd()) {
          break;
        }
        (*_it)->AcceptVisitor(*this);
      }
      return *this;
    }

    /// @todo to fix
    T *operator*() const {
      return static_cast<T *>(_it->get());
    }

    T *operator->() const {
      return &static_cast<T *>(_it->get());
    }

    bool operator!=(const RoadInfoIterator &rhs) const {
      return _it != rhs._it;
    }

    bool operator==(const RoadInfoIterator &rhs) const {
      return !((*this) != rhs);
    }

    bool IsAtEnd() const {
      return _it == _end;
    }

  private:

    void Visit(T &) {
      _success = true;
    }

    IT _it;

    IT _end;

    bool _success;
  };

  template <typename T, typename Container>
  static auto MakeRoadInfoIterator(const Container &c) {
    auto begin = std::begin(c);
    auto end = std::end(c);
    return RoadInfoIterator<T, decltype(begin)>(begin, end);
  }

  template <typename T, typename IT>
  static auto MakeRoadInfoIterator(IT begin, IT end) {
    return RoadInfoIterator<T, decltype(begin)>(begin, end);
  }

} // namespace element
} // namespace road
} // namespace carla
