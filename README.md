# queueing_model_validation
Software validation code for paper published in the Journal of Supercomputing
https://link.springer.com/article/10.1007/s11227-017-2109-4

Please reference as:
Salamí, Esther, Cristina Barrado, Antonia Gallardo, and Enric Pastor. "General queuing model for optimal seamless delivery of payload processing in multi-core processors." The Journal of Supercomputing 74, no. 1 (2018): 87-104.

This software is writen in Python and C++:
- Python is used to orchestrate the execution of several parallel services. Communication between services is based in LCM (see https://lcm-proj.github.io/)
- C++ is used to program a set of core kernels for a UAV image processing. The kernels use OpenCV 3.0 (see opencv.org/)

The C++ kernels are compiled for a Odroid multi-core board with Ubuntu Linux
