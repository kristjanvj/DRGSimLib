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

package drgsimlib.simulations;

import ned.IUnidirectionalChannel;
import drgsimlib.Global.Topology.Topology;
import drgsimlib.Nodes.Node;
import drgsimlib.Nodes.SensorNode;
import drgsimlib.Global.Topology.TopologyControl;
import drgsimlib.Global.NodeFactory.NodeFactory;
import drgsimlib.Support.GlobalObserver;

network gapscenario
{
    parameters:
        @display("bgb=697,453");
    submodules:
        topology: Topology {
            @display("p=67,55;i=misc/cloud");
        }
        factory: NodeFactory {
            @display("p=200,50;i=block/cogwheel");
        }        
        globalObserver: GlobalObserver {
            @display("p=234,118;i=block/user");
        }
        root: Node {
            @display("p=470,118;i=device/server");
        }
}
