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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "queue_sink_impl.h"

namespace gr {
  namespace router {


	/*
		For LTE: Need to change shared queues to a generic 'events' type.
		For the last queue_sink in the system (on root router), index will need to be preserved.
	*/

    queue_sink::sptr
    queue_sink::make(int item_size, boost::shared_ptr< boost::lockfree::queue< std::vector<float>* > > shared_queue, boost::shared_ptr< boost::lockfree::queue<float> > indexes, bool preserve_index)
    {
      return gnuradio::get_initial_sptr (new queue_sink_impl(item_size, shared_queue, indexes, preserve_index));
    }

    /*
     * The private constructor
     */
    queue_sink_impl::queue_sink_impl(int size, boost::shared_ptr< boost::lockfree::queue< std::vector<float>* > > shared_queue, boost::shared_ptr< boost::lockfree::queue<float> > index_queue, bool preserve_index)
      : gr::sync_block("queue_sink",
		      gr::io_signature::make(1, 1, sizeof(float)),
		      gr::io_signature::make(0, 0, 0))
    {
		queue = shared_queue; // Set shared_ptr queue
		indexes = index_queue; // Set shared_ptr index queue
		item_size = size; // Set size of items

		preserve = preserve_index; // Does index need to be preserved?

		if(!preserve)
			index_of_window = 0; // Set window index
	}

    /*
     * Our virtual destructor.
     */
    queue_sink_impl::~queue_sink_impl()
    {
    }

    // Return index; if index is preserved, used preserved value
    // Else, use value indexed from 0 ->
    int queue_sink_impl::get_index(){
    	if(preserve){
    		float index;
    		indexes->pop(index);
    		return index;
    	}
    	else{
    		return index_of_window++;
    	}
    }

    int
    queue_sink_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0]; // Input float buffer pointer

        // Add index if window doesnt already have one
        if(window->size() == 0){
        	window->push_back((float)get_index());
        }

        // Determine how many floats we need
	total_floats = window->size() + noutput_items - 1; // Determine number of floats available
	number_of_windows = total_floats / 1024; // determine number of windows we can make with floats
	left_over_values = total_floats % 1024; // Whats left after windowing the rest

	int remaining = 1025 - window->size(); // Number of floats needed to fill current window

	if(number_of_windows > 0){
			
		// Fill current window
		window->insert(window->end(), &in[0], &in[(remaining-1)]);

		in += remaining; // Update pointer

		// Push current window into queue
		queue->push(window); // Push window into queue
		queue_counter++;

		number_of_windows--;
		window->clear();

		// Populate windows and push into queue
		for(int i = 0; i < number_of_windows; i++){

			// Get index
			window->push_back(get_index());

			// Insert floats into the window		
			window->insert(window->end(), &in[0], &in[1023]);

			// Push window onto queue
			queue->push(window);
			queue_counter++;

			window->clear();
			in += 1024;
		}		

			// Put remaining floats in window array for next work execution
	}

	if(window->size() == 0)
		window->push_back(get_index());

	if(left_over_values > 0){
		window->insert(window->end(), &in[0], &in[(left_over_values-1)]);
	}

		//this->consume(0, noutput_items);

        return noutput_items;
    }

  } /* namespace router */
} /* namespace gr */

