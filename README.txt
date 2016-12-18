*** Wanshift - a game prototype ***

Table of Content
1.0 The Team
1.1 Members
2.0 Hardware/Software Requirements
2.1 Introduction
2.2 How the Game Works
2.3 Controls
3.0 Debug
3.1 The Debug Console
3.2 Domains
	3.2.1 SRVDEBUG
	3.2.2 GFX
	3.2.3 SYSTEM
	3.2.4 CAMERA
3.3 General Debug Commands
4.0 Intallation notes

1.0 --- The Team ---

1.1 Members
Rickard Fridvall
Stefan Grahn
Ossian Petri
Emil Bertilsson
Tobias Andersson
Martin Säll
Christopher Petersson
Jimmy Gustafsson

2.0 Hardware/Software Requirements
The game requires at minimum:
*256 MB VRAM
*DirectX 11
*Shader Model 5
*256 RAM
*Windows Vista or later
*500 MB disk space (not really though)

2.1 Introduction
WanShift is a simple survival game. The player controls a robot
in a dynamically created world. The player is free to explore the
world at his/hers own leasure. The world is made up of 5 different
biomes, each with their own characteristics.

2.2 How the Game Works
The robot is driven by energy. The amount of energy stored is represented
by the glowing patterns on the robots clothes. A bright white light signals
a healthy storage of energy. A red light signals that only 10% of the energy
storage is left.

The light will pulsate as the energy is drained/harvested by external forces.

A default consumption is always draining the energy slowly. Certain actions
will increase the energy consumption. The following actions will increase
the energy drain:

*Sprinting
*Using the headlight
*Wisps (bright coloured moving blobs) attacks

Energy can be restored by harvesting energy sources (bright blue floating blobs)

Additional energy drain is caused by moving from one biom to another. Different
biomes has different temperatures and the robot will slowly acclimatize by its own.
During this period the robot will drain at a higher rate. Acclimatizing will also 
allow the robot to move at a slightly higher pace.


2.3 Controls
The game uses the following controls:
*Movement - WASD
*Energy Harvest - 'e' (in close proximity to a energy resource)
*Headlights - 'f'
*Jump - SPACE
*Sprint - Left Shift (during movement)
*Exit - ESC

3.0 --- Debug ---

3.1 The Debug Console
Open the debug console by pressing '§'

3.2 Domains
In order to switch domains type 'switch domainName', where 'domainName' is one of
the following names:
* srvdebug
* gfx
* system
* camera

3.2.1 SRVDEBUG
Shows different textures in a separarate field. Used for debugging texture generation
and rendertargets. type 'show name' where 'name' corresponds to the handle name of that
resource.

3.2.2 GFX
Shows:
* Current Biom
* Biom blend values
* Total amount of used VRAM
* Amount of used VRAMfor ID3D11Buffers
* Amount of used VRAMfor ID3D11Texture2D + ID3D11Texture3D + BackBuffer
* Amount of used VRAMfor Shaderfiles

3.2.3 SYSTEM
Shows:
* Total amount of currently used RAM
* Largest RAM peak
* FPS

3.2.4 CAMERA
Shows:
* Camera Mode (free / locked)
* Avatar Position
* Camera Position
* Time of Day not working correctly in the current build)

3.2 General Debug Commands
* Debug Camera - 'cam free'
* Game Camera - 'cam lock'
(please note that due to a bug the user must first write 'cam lock' then
'cam free' in order to switch to debug camera)

4.0 Installation Notes
Double click the installation .exe and let install. Double click WanShift.exe in
the installation folder in order to play.
