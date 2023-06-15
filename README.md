# UEPhysicalModelingToolkit
Physical Modeling Library (Unreal Engine Plugin) including advanced Metasound math nodes and operators, signal processors, model Metasounds graphs (including machinery, nature, etc.), and more.

![PluginStrip](https://github.com/NiccoloAbate/UEPhysicalModelingToolkit/assets/27022723/05c523b5-172b-4f9e-8880-fe99903a2c9a)

YouTube Demonstration of the Plugin Available At: https://www.youtube.com/watch?v=mqzV9SYmdQ0

# Download / Installation
Download the packaged plugin zip or full project source under [Releases](https://github.com/NiccoloAbate/UEPhysicalModelingToolkit/releases/latest).

To install the plugin, simply copy the plugin folder from the zip under releases or directly from the project in the repository into a "Plugins" folder in your unreal project. Restart your project and the plugin should compile if necessary and be added to your project.

# Plugin Content Overview
* Metasound Nodes
  * Math Operators
    * Cos and Sin nodes (Audio AND Float) - Compute trig functions at float or audio rate (sample by sample).
    * Subtract operators (Float From Audio AND Audio From Float) - Audio subtraction operators for ease of access.
    * Divide operators (Audio AND Float By Audio) - Audio division operators which aren't possible with the standard node library.
  * Expression nodes (Audio AND Float) - Powerful arbitrary expression node allowing for concise, unlimited, and even runtime determined math computations at float or audio rate (sample by sample).
* Metasound Utility Patches
  * Wrap (Audio) - Expression "x - floor(x)".
* Metasound Examples (Sources, with corresponding Patches)
  * Engine Physical Model - Parameterized engine model, controlled primarilly by engine speed.
  * Housefly Physical Model - Simple but effective housefly model.
  * Lifeform Model - WIP lifeform model. Complicated model for generating lifeform sounds based on articulation parameters and physical characteristics of the lifeform.
  * Wind Physical Model - Complex wind model consisting of differenent individual wind type models (buildings, doorways, branches, and wires) which can be mixed for different types of wind.

# Math Operators
Various math operators and nodes not available in the standard Metasounds node library are critical for physical modeling and advanced processing.

![CosAudioExample](https://github.com/NiccoloAbate/UEPhysicalModelingToolkit/assets/27022723/fe5e6c80-c672-40cb-bcbf-9f988d1302fb)

# Expression Nodes
The arbitrary expression nodes (Audio AND Float) are invaluable nodes to have for physical modeling, as well as everyday Metasound productions.

![ExpressionNodeExample](https://github.com/NiccoloAbate/UEPhysicalModelingToolkit/assets/27022723/a3f81e8a-5884-4728-8373-1f4d23de2dea)

The node parses given string expressions (for 1 argument "x") (currently using the [TinyExpr library](https://github.com/codeplea/tinyexpr) to compile expressions) and then computes and outputs the expression at float or audio rate (sample by samples).
Specifically, the expression string is compiled when the string is updated, firing a trigger and error message out of the node in Metasounds as well as a log error if the expression is invalid.
The successfully compiled expression will computed and output, or the unchanged input (or 0 in the case of no input variables) will be output if there is no successfully compiled expression.

* Math Operators
  * "+", "-", "/", "*", and "%"
* Availabled Functions (more to be added)
  * min, max, clamp, abs, acos, asin, atan, atan2, ceil, cos, cosh, exp, floor, ln (calls to log), log (calls to log10), log10, pow, sin, sinh, sqrt, tan, tanh

