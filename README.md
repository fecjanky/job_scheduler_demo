# job_scheduler_demo [![Build Status](https://travis-ci.org/fecjanky/job_scheduler_demo.svg?branch=master)](https://travis-ci.org/fecjanky/job_scheduler_demo) [![Coverage Status](https://coveralls.io/repos/github/fecjanky/job_scheduler_demo/badge.svg?branch=master)](https://coveralls.io/github/fecjanky/job_scheduler_demo?branch=master) #

job scheduler demo, elements

* scheduler_lib, header only class template for representing a job graph and generating a job schedule based on topological ordering algorithm, and for parsing a simplified dot file
* scheduler, executable application that outputs the scheduling of a graph based on text input in simplified dot format
* test, various unit test cases
