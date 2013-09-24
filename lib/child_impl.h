/* -*- c++ -*- */
/* 
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_ROUTER_CHILD_IMPL_H
#define INCLUDED_ROUTER_CHILD_IMPL_H

#include "NetworkInterface.h"
#include <router/child.h>
#include <memory>
#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>
#include <vector>

namespace gr {
  namespace router {

    class child_impl : public child
    {
     private:
	

	int master_thread_index;
	boost::mutex index_lock;
	int get_index();

	int child_index;

	int number_of_children;
	bool d_finished;
	char * parent_hostname;


	boost::shared_ptr< boost::lockfree::queue< std::vector<float>* > > in_queue;
	float in_queue_counter;

	boost::shared_ptr< boost::lockfree::queue< std::vector<float>* > > out_queue;
	float out_queue_counter;

	int global_counter;
	boost::mutex global_lock;

	std::vector< std::vector <float> > out_vector;	

	boost::shared_ptr< boost::thread > d_thread;

	double * weights;

	NetworkInterface *connector;

	float index_of_window;
	float current_index;

	int number_of_output_items;

	int total_floats, number_of_windows, left_over_values;

	// Thread program
	void run();

	// Determine index of min child
	int min();


	// Global counter increment/decrement functions
	void increment();
	void decrement();


	// Return global counter value
	int get_weight();

	// Compare function for SORT
	//bool compare_by_index(const vector<float> &a, const vector<float> &b);


     public:
      child_impl(int number_of_children, int child_index, char* hostname, boost::shared_ptr< boost::lockfree::queue< std::vector<float>* > > in_queue,
	boost::shared_ptr< boost::lockfree::queue< std::vector<float>* > > out_queue);
      ~child_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace router
} // namespace gr

#endif /* INCLUDED_ROUTER_CHILD_IMPL_H */

