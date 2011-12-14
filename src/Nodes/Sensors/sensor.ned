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

package drgsimlib.Nodes.Sensors;

//
// A datasource used to simulate a sensor. Use with the SensorNode compound module.
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
simple Sensor
{
    parameters:
        bool enabled = default(false);
        double valueInitial;                     // The initial value of the sensor (minus noise)
        double valueMultiplier;                  // The slope of the sensor trend
        volatile double valueNoiseDistribution;  // Use a random distribution in the ini file
        volatile double updateTimeDistribution;  // Use a random distribution in the ini file
    gates:
        output out;
        input  in;
}
