/*
 * Copyright (C) 2017-2018 Trent Houliston <trent@houliston.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef VISUALMESH_GENERATOR_QUADPIZZA_HPP
#  define VISUALMESH_GENERATOR_QUADPIZZA_HPP

#  include <array>
#  include <vector>
#  include "mesh/node.hpp"

namespace visualmesh {
namespace generator {

  template <typename Scalar>
  struct QuadPizza {
  private:
    static inline vec3<Scalar> unit_vector(const Scalar& sin_phi, const Scalar& cos_phi, const Scalar& theta) {
      return vec3<Scalar>{{std::cos(theta) * sin_phi, std::sin(theta) * sin_phi, -cos_phi}};
    }

  public:
    template <typename Shape>
    static std::vector<Node<Scalar>> generate(const Shape& shape,
                                              const Scalar& h,
                                              const Scalar& k,
                                              const Scalar& max_distance) {

      // Loop through until we reach our max distance
      std::vector<Node<Scalar>> nodes;

      // L, T, R, B
      int LEFT  = 0;
      int TOP   = 1;
      int RIGHT = 2;
      int BELOW = 3;

      // auto join_row_neighbours = [&](Node<Scalar>& node,
      //                                const int index,
      //                                const in end,
      //                                const int LEFT,
      //                                const int RIGHT,
      //                                const int number_of_points,
      //                                const bool clockwise) {
      //   int left;
      //   int right;

      //   if (clockwise == false) {
      //     left  = RIGHT;
      //     right = LEFT;
      //   }
      //   else {
      //     left  = LEFT;
      //     right = RIGHT;
      //   }

      //   if (index == 0) {
      //     node.neighbours[right] = end + number_of_points - 1;
      //     node.neighbours[left]  = end + 1;
      //   }
      //   else if (index == number_of_points - 1) {
      //     node.neighbours[right] = end + number_of_points - 2;
      //     node.neighbours[left]  = end + 0;
      //   }
      //   else {
      //     node.neighbours[right] = end + index - 1;
      //     node.neighbours[left]  = end + index + 1;
      //   }
      // };

      std::vector<Scalar> dtheta;
      std::array<int, 4> first_neighbours;
      first_neighbours[0] = 2;
      first_neighbours[1] = 3;
      first_neighbours[2] = 0;
      first_neighbours[3] = 1;

      Scalar phi_first = shape.phi(1 / k, h) / 2;
      dtheta.push_back(2 * M_PI / 4);
      bool clock = true;

      for (int j = 0; j < 4; ++j) {
        Node<Scalar> first;
        first.ray               = unit_vector(std::sin(phi_first / 2), std::cos(phi_first / 2), j * (2 * M_PI / 4));
        first.neighbours[BELOW] = first_neighbours[j];
        // join_row_neighbours(first, j, 0, LEFT, RIGHT, 4, clock);
        first.neighbours[LEFT]  = (j + 1) % 4;
        first.neighbours[RIGHT] = (j - 1) % 4;
        nodes.push_back(std::move(first));
      }

      std::vector<int> number_points;
      number_points.emplace_back(4);
      int running_index;
      running_index = nodes.size();

      std::vector<int> origin_number_points;
      origin_number_points.emplace_back(0);
      origin_number_points.emplace_back(8);
      for (int i = 0; i < k; ++i) {
        origin_number_points.emplace_back(12 + 8 * i);
      }


      for (int v = 1; h * std::tan(shape.phi(v / k, h)) < max_distance; ++v) {
        Scalar phi_next = shape.phi(v / k, h);


        int begin = running_index - number_points.back();
        int end   = running_index;
        // hack
        int one = std::round(std::pow(-1, v));
        // bool clockwise = v % 2 == 0;

        Scalar theta_next;
        int number_points_now;
        int number_points_next;
        int number_difference;
        bool every_one = false;
        bool growing   = false;

        // precalculate the number of points vector according to distribution and origin fix

        // if (v < k + 1) {
        if (v < k - 1) {

          number_points_now  = number_points[v - 1];
          number_points_next = origin_number_points[v];
          // std::cout << "number_points_now" << number_points_now << std::endl;
          // std::cout << "number_points_next" << number_points_next << std::endl;
          number_difference = number_points_next - number_points_now;
          // std::cout << "number_difference: " << number_difference << " "
          //           << "v: " << v << std::endl;

          theta_next = 2 * M_PI / number_points_next;
          // std::cout << "theta_next: " << theta_next << std::endl;
        }
        else {

          number_points_now = number_points[v - 1];
          // floor vs ceil
          number_points_next = std::ceil((2 * M_PI * k) / shape.theta(phi_next, h));
          // std::cout << "No: " << number_points_next << std::endl;
          number_difference = number_points_next - number_points_now;
          theta_next        = 2 * M_PI / number_points_next;
          // std::cout << "theta_next: " << theta_next << std::endl;
          // std::cout << "number_difference: " << number_difference << " "
          // << "v: " << v << std::endl;
        }

        int distribution;

        if (number_difference == 0) {
          // std::cout << "Difference is zero: " << number_difference << " ring number: " << v
          //           << " distribution: " << distribution << std::endl;
          distribution = 0;

          theta_next         = 2 * M_PI / number_points_now;
          number_points_next = number_points_now;
          number_points.emplace_back(number_points_now);
        }
        else if (number_difference == 1) {
          growing      = true;
          distribution = 1;
          // std::cout << "Difference is 1: " << number_difference << " ring number: " << v << std::endl;
          number_points_next = 1 + number_points_now;
          number_points.emplace_back(1 + number_points_now);
          theta_next        = 2 * M_PI / (1 + number_points_now);
          number_difference = 1;
        }
        else if (number_difference > 1) {
          growing = true;
          if (number_difference < number_points_now) {
            // make sure this is correct. safe to get rid of floor?
            distribution = number_points_now / number_difference;
            // std::cout << "Difference is reasonable: " << number_difference << " ring number: " << v
            //           << " distribution: " << distribution << std::endl;
            number_points.emplace_back(number_points_next);
            if (distribution == 1) {
              every_one = true;
              // std::cout << "I ran" << std::endl;
            }
          }
          else {
            // number_difference >= number_points_now
            distribution = 1;
            // std::cout << "Difference is greater than generating ring: " << number_difference << " ring number: " <<
            // v
            //<< std::endl;
            number_points_next = 2 * number_points_now;
            number_points.emplace_back(2 * number_points_now);
            theta_next        = M_PI / number_points_now;
            number_difference = number_points_now;
          }
        }
        else {
          // number_difference is negative
          distribution       = 0;
          theta_next         = 2 * M_PI / number_points_now;
          number_points_next = number_points_now;
          number_points.emplace_back(number_points_now);
          // std::cout << "Difference is negative: " << number_difference << " ring number: " << v
          //           << " distribution: " << distribution << std::endl;
        }

        dtheta.push_back(theta_next);
        std::vector<int> indices;
        // std::cout << "begin" << begin << std ::endl;
        // std::cout << "end" << end << std ::endl;
        for (int i = begin; i < end; ++i) {
          indices.push_back(i);
        }

        // condense this
        std::vector<int> vector_of_indices;
        if (v == 1) {

          vector_of_indices.push_back(indices[0]);
          for (int m = indices.size() - 1; m > 0; --m) {
            vector_of_indices.push_back(indices[m]);
          }
        }
        else {
          vector_of_indices.push_back(indices[1]);
          vector_of_indices.push_back(indices[0]);
          for (int m = indices.size() - 1; m > 1; --m) {
            vector_of_indices.push_back(indices[m]);
          }
        }


        int relative_index_now  = 0;
        int relative_index_next = 0;
        int number_splits       = 0;


        Scalar theta_offset;
        theta_offset = -1 * one * dtheta.back();

        for (auto it = vector_of_indices.begin(); it != vector_of_indices.end(); ++it) {

          Node<Scalar> new_node;

          new_node.ray =
            unit_vector(std::sin(phi_next), std::cos(phi_next), theta_offset + one * relative_index_next * theta_next);

          // join_row_neighbours(new_node, relative_index_next, end, LEFT, RIGHT, number_points_next, clockwise);
          // odd v generates clockwise, even v generates anti-clockwise.
          new_node.neighbours[LEFT]  = end + ((relative_index_next + one) % number_points_next);
          new_node.neighbours[RIGHT] = end + ((relative_index_next - one) % number_points_next);


          new_node.neighbours[BELOW] = *it;
          // nodes[*it].neighbours[TOP] = end + relative_index_next % number_points_next;

          nodes[*it].neighbours[TOP] = end + relative_index_next;

          nodes.push_back(std::move(new_node));
          relative_index_next += 1;

          // *************** Generate Second Node ***********************
          if (growing) {
            if (every_one == true) {
              if (number_splits <= number_points_now - number_difference) { distribution = 2; }
              else {
                distribution = 1;
              }
            }

            // split every point of according to the distribution until difference is reached, or split every point
            // to exactly double the points
            bool distribute = false;
            distribute      = relative_index_now % distribution == 0;
            bool splits     = false;
            splits          = number_splits < number_difference;

            if ((distribute || distribution == 1) && splits) {
              // std::cout << "Graph is growing!" << std::endl;
              // std::cout << "count: " << count << std::endl;
              Node<Scalar> second_new_node;

              second_new_node.ray = unit_vector(
                std::sin(phi_next), std::cos(phi_next), theta_offset + one * relative_index_next * theta_next);

              // join_row_neighbours(
              //  second_new_node, relative_index_next, end, LEFT, RIGHT, number_points_next, clockwise);
              second_new_node.neighbours[LEFT]  = end + ((relative_index_next + one) % number_points_next);
              second_new_node.neighbours[RIGHT] = end + ((relative_index_next - one) % number_points_next);
              second_new_node.neighbours[BELOW] = *it;

              nodes.push_back(std::move(second_new_node));

              number_splits += 1;
              relative_index_next += 1;
            }
          }
          // *********************************************************************************

          relative_index_now += 1;
          // std::cout << "MESHPOINTS" << meshpoints.size() << std::endl;
          // std::cout << "relative_index_now: " << relative_index_now << std::endl;
          // std::cout << "relative_index_next: " << relative_index_next << std::endl;
        }


        running_index = nodes.size();
        // std::cout << "meshpoints: " << meshpoints.size() << std::endl;
      }

      // specify the neighbours of the last ring of points
      for (int i = (nodes.size() - number_points.back()); i < nodes.size(); ++i) {
        nodes[i].neighbours[TOP] = i;
      }

      // // separate conversion for first ring
      // for (int i = 0; i < number_points[0]; ++i) {
      //   // size_t sum_top = number_points_temp[1] + 1;
      //   int sum_top = number_points[0];

      //   nodes[i].neighbours[TOP]   = sum_top + nodes[i].neighbours[TOP];
      //   nodes[i].neighbours[LEFT]  = nodes[i].neighbours[LEFT];
      //   nodes[i].neighbours[RIGHT] = nodes[i].neighbours[RIGHT];
      // }


      // // convert local neighbour indices to global
      // for (int i = number_points[0]; i < (nodes.size() - number_points.back()); ++i) {
      //   int sum_top =
      //     std::accumulate(number_points.begin(), std::next(number_points.begin(), nodes[i].phi_number + 1), 0);
      //   int sum_now = std::accumulate(number_points.begin(), std::next(number_points.begin(), nodes[i].phi_number),
      //   0);

      //   nodes[i].neighbours[TOP]   = sum_top + nodes[i].neighbours[TOP];
      //   nodes[i].neighbours[LEFT]  = sum_now + nodes[i].neighbours[LEFT];
      //   nodes[i].neighbours[RIGHT] = sum_now + nodes[i].neighbours[RIGHT];
      // }


      // // Calculate our unit vectors for all our mesh points with x facing forward and z up
      // for (int n = 0; n < meshpoints.size(); ++n) {
      //   meshpoints[n].ray = {{
      //     std::cos(meshpoints[n].current[1]) * std::sin(meshpoints[n].current[0]),  //
      //     std::sin(meshpoints[n].current[1]) * std::sin(meshpoints[n].current[0]),  //
      //     -std::cos(meshpoints[n].current[0]),                                      //
      //     Scalar(0.0)                                                               //
      //   }};
      //   //   std::cout << "meshpoint: " << n << ": " << meshpoints[n].ray[0] << ", " << meshpoints[n].ray[1] << ", "
      //   //             << meshpoints[n].ray[2] << std::endl;
      // }

      for (int i = 0; i < nodes.size(); ++i) {
        nodes[i].neighbours[4] = nodes[i].neighbours[BELOW];
        nodes[i].neighbours[5] = nodes[i].neighbours[BELOW];
      }


      return nodes;
    }  // namespace generator
  };   // namespace generator

}  // namespace generator
}  // namespace visualmesh

#endif  // VISUALMESH_GENERATOR_QUADPIZZA_HPP


// auto join_above_neighbours = [&](Meshpoint<Scalar>& meshpoint,
//                                  const int index,
//                                  const int TOP_LEFT,
//                                  const int TOP_RIGHT,
//                                  const int number_of_points,
//                                  const bool clockwise) {
//   int top_left;
//   int top_right;

//   if (clockwise == false) {
//     top_left  = TOP_RIGHT;
//     top_right = TOP_LEFT;
//   }
//   else {
//     top_left  = TOP_LEFT;
//     top_right = TOP_RIGHT;
//   }

//   if (index == number_of_points - 1) {
//     meshpoint.neighbours[top_right] = index;
//     meshpoint.neighbours[top_left]  = 0;
//   }
//   else {
//     meshpoint.neighbours[top_right] = index;
//     meshpoint.neighbours[top_left]  = index + 1;
//   }
// };


// // merge

// for (int it = 0; it < ring.size(); ++it) {
//   nodes.push_back(std::move(ring[it]));
// }


// std::vector<double> distribution_vec;
// std::vector<size_t> number_by_phi;
// number_by_phi.push_back(4);
// number_by_phi.push_back(std::ceil((2 * M_PI * k) / shape.theta(shape.phi((v + k / 2) / k, h), h)));


// if (phi_next > 1.55) {
//   std::cout << "v: " << v << std::endl;
//   std::cout << "phi_next: " << std::endl;
//   printf("%f \n", phi_next);
//   std::cout << "h: " << std::endl;
//   printf("%f \n", h);
// }
// if (theta_next < 0.00005) {
//   std::cout << "v: " << v << std::endl;
//   std::cout << "theta: " << std::endl;
//   printf("%f \n", theta_next);
//   std::cout << "h: " << std::endl;
//   printf("%f \n", h);
// }


// for (size_t i = 0; i < distribution_vec.size(); ++i) {
//   printf("%2f \n", distribution_vec[i]);
// }

// for (int i = 0; i < meshpoints.size(); ++i) {
//   // std::cout << "i: " << i << std::endl;
//   int neighbour = meshpoints[i].neighbours[BELOW];
//   double theta1 = meshpoints[i].current[1];

//   if (!std::isfinite(theta1)) { printf("%f \n", theta1); }

//   double theta2 = meshpoints[neighbour].current[1];

//   double abs_theta1 = std::abs(meshpoints[i].current[1]);
//   double abs_theta2 = std::abs(meshpoints[neighbour].current[1]);

//   // normalise
//   double norm_theta1 = abs_theta1 - (2 * M_PI * (std::floor(abs_theta1 / (2 * M_PI))));
//   double norm_theta2 = abs_theta2 - (2 * M_PI * (std::floor(abs_theta2 / (2 * M_PI))));

//   // correct the direction

//   if (theta1 < 0) { norm_theta1 = 2 * M_PI - norm_theta1; }
//   if (theta2 < 0) { norm_theta2 = 2 * M_PI - norm_theta2; }

//   meshpoints[i].current[1] = norm_theta1;

//   // if (i == 53) {
//   //   std::cout << "*********************************************" << std::endl;
//   //   printf("This: %f and %f and %f and %f \n", theta1, theta2, norm_theta1, norm_theta2);
//   // }
//   double error;

//   error = std::min(2 * M_PI - std::abs(norm_theta1 - norm_theta2), std::abs(norm_theta1 - norm_theta2));

//   // printf("%f \n", norm_theta1);
//   // std::cout << i << ": " << neighbour << std::endl;
// }

// print out mesh points
// for (int i = 0; i < nodes.size(); ++i) {
//   std::cout << "meshpoint: " << i << ": " << nodes[i].neighbours[LEFT] << ", "
//             << nodes[i].neighbours[RIGHT] << ", " << nodes[i].neighbours[TOP] << ", " <<
//             nodes[i].neighbours[BELOW] << std::endl;

// }
