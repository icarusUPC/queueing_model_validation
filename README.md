# queueing_model_validation
Software validation code for paper to be published in the Journal of Supercomputing

This software is writen in Python and C++:
- Python is used to orchestrate the execution of several parallel services. Communication between services is based in LCM (see https://lcm-proj.github.io/)
- C++ is used to program a set of core kernels for a UAV image processing. The kernels use OpenCV 3.0 (see opencv.org/)
