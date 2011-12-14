// ****************************************************************************
//
// DRGSim -- Dynamic Random Graph Simulation Components
//
// ****************************************************************************
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package drgsimlib.Nodes;
import drgsimlib.Protocols.IProtocol;
import drgsimlib.Global.NICs.TopologyControlNIC;
import drgsimlib.Nodes.Sensors.Sensor;

//
// Sensor node compound module for use in a DRG simulation.
// Contains a sensor, generic protocol module and a TopologyControlNIC.
// Plug in a protocol such as GAP in the ini file.
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @todo document
//
module SensorNode
{
    parameters:
        int nodeid;
        string protocolName = default("Ping");
        @display("bgb=189,185");
    submodules:
        nic: TopologyControlNIC {
            parameters:
                @display("p=123,138;i=block/ifcard");
        }
        protocol: <protocolName> like IProtocol {
            parameters:
                nodeid = nodeid;
                @display("p=123,39;i=block/cogwheel");
        }
        sensor: Sensor {
            @display("p=33,39;i=misc/thermometer");
        }
    connections:
        nic.upperOut --> protocol.lowerIn;
        protocol.lowerOut --> nic.upperIn;
        sensor.out --> protocol.auxIn;
        protocol.auxOut --> sensor.in;
        nic.upperControlOut --> protocol.lowerControlIn;
        protocol.lowerControlOut --> nic.upperControlIn;
}
