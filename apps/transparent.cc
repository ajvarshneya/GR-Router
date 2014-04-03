/*
 * Copyright 2013 Tommy Tracy II (University of Virginia) 
 *
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

 /*
  This is a test application for the GR-ROUTER library.
  It is used to test the throughput of a single machine, and use it as a base case for distributed speedup metrics.
*/


// Include header files for each block used in flowgraph
#include <gnuradio/blocks/wavfile_source.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/blocks/throttle.h>
#include <boost/lockfree/queue.hpp>
#include <router/queue_source.h>
#include <gnuradio/top_block.h>
#include <router/queue_sink.h> 
#include <router/throughput.h>
#include <router/throughput_sink.h>
#include <boost/thread.hpp>
#include "fft_ifft.h"
#include <vector>
#include <cstdio>

using namespace gr;

int main(int argc, char **argv)
{

  double throughput_value = 1e6; // Set the throughput of the throttle
 
  gr::top_block_sptr tb_1 = gr::make_top_block("transparent");

  const char* in_file_name = "inputs/out.wav";
  const char* out_file_name = "/dev/null";

  //boost::lockfree::queue< std::vector<float>*, boost::lockfree::fixed_sized<true> > input_queue(100);
  //boost::lockfree::queue< std::vector<float>*, boost::lockfree::fixed_sized<true> > output_queue(100);

  gr::blocks::wavfile_source::sptr wavfile_source = gr::blocks::wavfile_source::make(in_file_name, true); // input file source (WAV) [input_file, repeat=false]
  gr::blocks::file_sink::sptr file_sink = gr::blocks::file_sink::make(sizeof(float), out_file_name); // output file sink (BIN) [sizeof(float), output_file]

  gr::blocks::throttle::sptr throttle_0 = gr::blocks::throttle::make(sizeof(float), throughput_value);
  //gr::blocks::throttle::sptr throttle_1 = gr::blocks::throttle::make(sizeof(float), throughput_value);
  //gr::blocks::throttle::sptr throttle_2 = gr::blocks::throttle::make(sizeof(float), throughput_value);

  //gr::router::queue_sink::sptr input_queue_sink = gr::router::queue_sink::make(sizeof(float), input_queue, false); // input queue sink [sizeof(float), input_queue, preserve index after = true, throughput=10e6]
  //gr::router::queue_source::sptr input_queue_source = gr::router::queue_source::make(sizeof(float), input_queue, false, false); // input queue source [sizeof(float), input_queue, preserve index = true, order = true]

  //gr::router::queue_sink::sptr output_queue_sink = gr::router::queue_sink::make(sizeof(float), output_queue, false);
  //gr::router::queue_source::sptr output_queue_source = gr::router::queue_source::make(sizeof(float), output_queue, false, false); // Preserve index, order data, write file


  //gr::router::throughput::sptr throughput_0 = gr::router::throughput::make(sizeof(float), 10, 0);
  gr::router::throughput_sink::sptr throughput_sink = gr::router::throughput_sink::make(sizeof(float), 10, 0);
  //gr::router::throughput::sptr throughput_1 = gr::router::throughput::make(sizeof(float), 10, 1);
  //gr::router::throughput::sptr throughput_2 = gr::router::throughput::make(sizeof(float), 10, 2);
  
   //tb_1->connect(wavfile_source, 0, throughput, 0);
   //tb_1->connect(throughput, 0, input_queue_sink, 0);
  tb_1->connect(wavfile_source, 0, throttle_0, 0);
  //tb_1->connect(throttle_0, 0, throughput_0, 0);
  //tb_1->connect(throughput_0, 0, input_queue_sink, 0);
   //tb_1->connect(throttle_0, 0, input_queue_sink, 0);
   //tb_1->connect(input_queue_source, 0, throughput, 0);
  //tb_1->connect(throughput, 0, file_sink, 0);
   //tb_1->connect(input_queue_source, 0, file_sink, 0);

  //tb_1->connect(wavfile_source, 0, input_queue_source, 0);

  //tb_1->connect(wavfile_source, 0, file_sink, 0);

  //tb_1->connect(wavfile_source, 0, input_queue_sink, 0);

  std::vector<fft_ifft_sptr> ffts;
  for(int i = 0; i < 50; i++){
    ffts.push_back(fft_ifft_make(1024));
    if(i == 0){
      //tb_1->connect(input_queue_source, 0, ffts.at(0), 0);
      tb_1->connect(throttle_0, 0, ffts.at(0), 0);
    }
    else{
      tb_1->connect(ffts.at(i-1), 0, ffts.at(i), 0);
    }
  }
  //tb_1->connect(ffts.at(ffts.size()-1), 0, throttle_1, 0);
  //tb_1->connect(throttle_1, 0, throughput_1, 0);
  //tb_1->connect(throughput_1, 0, output_queue_sink, 0);
  //tb_1->connect(throttle_1, 0, output_queue_sink , 0);
  //tb_1->connect(ffts.at(ffts.size()-1), 0, output_queue_sink, 0);

  tb_1->connect(ffts.at(ffts.size()-1), 0, throughput_sink, 0);
  //tb_1->connect(throughput_0, 0, output_queue_sink, 0);
  //tb_1->connect(output_queue_source, 0, throughput_0, 0);
  //tb_1->connect(throttle_2, 0, throughput_2, 0);
  tb_1->connect(ffts.at(ffts.size()-1), 0, file_sink, 0);
  //tb_1->connect(output_queue_source, 0, file_sink, 0);
  // tb_1->connect(input_queue_source, 0, file_sink, 0);

  tb_1->run();

  return 0;
}
