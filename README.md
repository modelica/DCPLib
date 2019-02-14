# DCPLib #

DCPLib is a C++ implementation of the Distributed Co-Simulation Protocol (DCP). It provides an API for slaves (e. g. simulators) and master tools. 

## Packages ##
| Package          | Description | Dependencies                           |
|------------------|-------------|----------------------------------------|
| DCPLib::Core     | Containing all common classes, like constants, PDU definitions etc.            |                                        |
| DCPLib::Master   | Containing all classes relevant to build a master tool for DCP            | DCPLib::Core                           |
| DCPLib::Slave    | Containing all classes relevant to implemant an DCP slave.             | DCPLib::Core                           |
| DCPLib::Ethernet | Classes to add UDP_IPv4 or TCP support to the DCLib::master or DCPLib::slave package            | Asio standalone, DCPLib::Core, Threads |
| DCPLib::Xml | Classes to read/write a slave description from/to xml (dcpx).           | DCPLib::Core, Xerces-c |
| DCPLib::Zip | Classes to read/write slave description from/to zip           | DCPLib::Core, DCPLib::Xml, Xerces-c, LibZip|
## Wiki ##
For hints how to use this library, take a look at the [wiki](https://github.com/ChKater/DCPLib/wiki) pages
## Example ##
See [example](example) for a implementation of a master and slave.

## Acknowledgement ##
- 2018 - 2019: The work on this library was done by the Simulation & Modelling Group of the Leibniz Universität Hannover.
- 2015 - 2018: The work on this library was done in the contex of the ITEA3 Project ACOSAR (N◦14004) by the Simulation & Modelling Group of the Leibniz Universität Hannover. The ACOSAR project was partially funded by the Austrian Competence Centers for Excellent Technologies (COMET) program, the Austrian Research Promotion Agency (FFG), and by the German Federal Ministry of Education and Research (BMBF) under the support code 01lS15033A.
