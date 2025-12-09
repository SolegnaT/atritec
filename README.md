# Contents
- `atrisense.bin`: given data
- `atritec.cpp`: code for solving the following tasks   
  Task 1. Decode the Atrisense binary into structured records.  
  Task 2. Convert each record into Cartesian coordinates (x, y, z).  
- `atritec_main.cpp`: entrypoint for running the code in `atritec.cpp` as a CLI tool.
- `atritec_test.cpp`: entrypoint for running tests on the code in `atritec.cpp`
- `atritec.py`: code for solving Task 3:  (Optional) Visualize the resulting point cloud in any tool you prefer (foxglove, other point cloud viewers).
- `coord_conversion.jpg`: derivation of the conversion from spherical coordinates to cartesian coordinates
- `setup.bat`: script for setting up mvsc compiler on my machine
- `build.bat`: script for building on my machine
- `.github/workflows/blank.yml`: github rules for compiling `atritec_test.cpp` and running the resulting executable
- `raddbg.exe`: my currently preferred debugger for C/C++ code
- `requirements.txt`: Dependences used by `atritec.py`
- `task3.jpg`: screenshot of Foxglove visualization from Task 3
- `clean.bat`: script to clean up build artifacts

# Requirements
- A C++ compiler.
- Python
- Foxglove account
- The python libraries specified in `requirements.txt`

# Solution to Task 1 and Task 2:
- Open a command terminal: `cmd.exe`
- Setup compiler, change path in `setup.bat` to fit your machine : `setup.bat`
- Compile `atritec_test.cpp` and `atritec_main.cpp`: `build.bat`
- Solve Task 1 and Task 2 by running the command line tool to generate `output.bin`: `atritec_main`

# Solution to Task 3: 
- First solve Task 1 and Task 2 to generate `output.bin`.
- Setup a python environment: `python -m venv pyenv`
- Activate the environment: `pyenv\Scripts\activate`
- Install dependencies: `pip install -r requirements.txt`
- Generate MCAP data `output.mcap`: `python atritec.py`
- Open up Foxglove in the browser and load `output.mcap`.
