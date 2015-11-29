# AFMulator_RigidBody

this is Rigid-Body Classical Molecular-Dynamics simulator of High resolution Atomic force microscopy images

## Usage 

Parameters for the application are loaded from configuration files stored in directory InputData, configuration files form a hierarchy:

#### confFilesCommander:
* atomTypesFile       - specification for chemical elements
* scanningParams      - dimensions of the scanning area, scanning mode, convergence parameters etc.
* moleculeParams:     - structure of molecules, their number etc.
..* listOfFileNames   - files where individual molecule structures are stored
..* geometryFile      - geometry of the system, i.e. position and orientation of each molecule
* tipParams:          - specification of the tip, force coefficient constants, which molecule is bound to the tip etc.
..* probeMolFileName  - structure of the probe molecule
..* tipSettimgFile    - force coefficient constants etc. 

#### Behaviour of the system depends tremendously on the scanning mode:
* SCANNING_NONE_VIEW - no relaxation or scanning takes place, just to see how the geometry of the system looks like
* SCANNING_NONE      - no scanning, only relaxation
* SCANNING_ONE       - just one z-direction movement of the scanning tip at a specified location, no 2D raster output is generated
* SCANNING_RASTER    - 3D scanning, almost all the functionality of the application is present in this mode
* SCANNING_REPLAY    - provided already generated data from raster scanning mode this mode imports them and replays the scanning process; one can move the scanning tip at will, graphics then shows how the molecules move according to the relaxation during the scanning; one should keep in mind in this mode, that all the configuration files used for generation of the data should be present unmodified even in this mode since the only thing imported are positions and orientations of relevant molecules; their structure or scanning area dimensions are loaded from the original configuration files


#### objects:

* PhysicalSystem - connects molecules, geometry, relaxation process and scanning process
* Screen         - responsible for majority of graphics
