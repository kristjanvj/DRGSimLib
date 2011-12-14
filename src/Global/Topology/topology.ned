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

package drgsimlib.Global.Topology;
import drgsimlib.Global.Topology.Generators.IBasicGenerator;

//
// The main random graph topology module
//
// A single Topology node must be instantiated at the scenario level
// in all DRG simulations 
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @see ToplogyControl
// 
module Topology
{
    parameters:
        string topologyGenerator = default(FullyConnectedGenerator);
        int updateInterval @unit("s") = default(1s);
        bool saveSnapshot = default(false);
        bool recordDuringInitialization = default(false);
        string snapshotFile = default("");
        string logFileName = default("");
        @display("bgb=279,114");
    submodules:
        generator: <topologyGenerator> like IBasicGenerator {
            @display("p=192,50;i=block/control");
        }
        topologyControl: TopologyControl {
            parameters:
                updateInterval = updateInterval;
                snapshotFile = snapshotFile;
                saveSnapshot = saveSnapshot;
                recordDuringInitialization = recordDuringInitialization;
                logFileName = logFileName;
                @display("p=78,50;i=block/cogwheel");
        }
}
