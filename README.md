# Near-Optimal Knowledge-Free Resilient Leader Election

This repository contains reproducible experiments (in FCPP) and algorithms (in C++, for finding the best parameter _g_) for a leader election algorithm submitted to [Automatica](https://www.journals.elsevier.com/automatica).
For any issues with reproducing the experiments, please contact [Giorgio Audrito](mailto:giorgio.audrito@gmail.com).

This FCPP translation has been presented at [ACSOS 2020 - 1st IEEE International Conference on Autonomic Computing and Self-Organizing Systems](https://conf.researchr.org/home/acsos-2020) through [this paper](http://giorgio.audrito.info/static/fcpp.pdf). For any issues with reproducing the experiments, please contact [Giorgio Audrito](mailto:giorgio.audrito@unito.it).

## Getting Started

### References

- FCPP main website: [https://fcpp.github.io](https://fcpp.github.io).
- FCPP documentation: [http://fcpp-doc.surge.sh](http://fcpp-doc.surge.sh).
- FCPP presentation in [this paper](http://giorgio.audrito.info/static/fcpp.pdf)
- FCPP sources: [https://github.com/fcpp/fcpp](https://github.com/fcpp/fcpp).

### Vagrant

Download Vagrant from [https://www.vagrantup.com](https://www.vagrantup.com), then type the following commands in a terminal:
```
vagrant up
vagrant ssh
cd fcpp
./make.sh run -O experiment
./make.sh run -O parameter
```
Then you should get output about building the experiments and running them (in the Vagrant virtual machine). After that you can exit and stop the virtual machine through:
```
exit
vagrant halt
```

### Docker

Download Docker from [https://www.docker.com](https://www.docker.com), then download the Docker container from GitHub by typing the following command in a terminal:
```
docker pull docker.pkg.github.com/fcpp/fcpp/container:1.0
```
Alternatively, you can build the container yourself with the following command:
```
docker build -t docker.pkg.github.com/fcpp/fcpp/container:1.0 .
```
Once you have the Docker container locally available, type the following commands:
```
docker run -it --volume $PWD:/fcpp --workdir /fcpp docker.pkg.github.com/fcpp/fcpp/container:1.0 bash
./make.sh run -O experiment
./make.sh run -O parameter
```
Then you should get output about building the experiments and running them (in the Docker container). After that you can exit and stop the container through:
```
exit
```

### Custom Build

In order to get started on your machine you need the following installed:

- [Bazel](https://bazel.build) (tested with version 2.1.0)
- [GCC](https://gcc.gnu.org) (tested with version 9.3.0)

Once you have them installed, you should be able to run:
```
./make.sh gcc run -O experiment
./make.sh gcc run -O parameter
```
getting output about building the experiments and running them.
