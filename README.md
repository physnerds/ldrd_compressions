# compressiontests
 This is feature/lcrc branch.

## Introduction
This git project provides tools to create synthetic waveform and uses different intelligent compression frameworks to look at the compression and decompression of those synthetic waveform data. 

## Checkout code from github
We will be using the lcrc branch for the development. As the code evolves, we will merge with the main branch.

### Download the project
```git clone git@github.com:physnerds/compressiontests.git```

### Checkout the lcrc branch.
```git checkout feature/lcrc```

## Waveform generator codes
There are python macros that generates synthetic waveforms.
1. write_hdf5data.py: This creates the synthetic waveforms and writes into hdf5 file.

2. analyze_hdf5.ipynb: This jupyter-notebook has an example of analyzing the synthetic waveforms that are created by write_hdf5data.py. 
Usually jupyter-notebook comes with ROOT installation (Instruction on ROOT installation is given below.)


## Requirements

## Creat Conda environment and install all dependencies.

1. In LCRC machines you should be able to find conda or anaconda. 

(I would recommend to install cmake before installing anything else so that there are no conflicts down the line between ROOT build and other builds 
```https://anaconda.org/anaconda/cmake```
)

2. ROOT installation via Conda is given here (https://root.cern/install/#install-via-a-package-manager)
It also gives example on how to create a conda environment inside which you can install all the required dependencies. 

3. On the same environment (that you created when you installed ROOT), install HDF5.
https://anaconda.org/anaconda/hdf5

4. On the same environment install protobuff via conda (needed by MGARD)


5. You will need h5py, a python API for HDF5.
```pip3 install h5py```

## Next we install the compression frameworks

Note, for all compression frameworks build, it would be easier down the line if the build files are copied to the respective install directory. 

5. SZ3 (https://github.com/szcompressor/SZ3)
   Build SZ3 with bundled in zstd compression for the sake of simplicity. See the configure file in SZ3 repository for more information on installation.

6. MGARD (https://github.com/CODARcode/MGARD.git) 
   Follow the readme document to install MGARD. 

7. IDEALEM (https://datagrid.lbl.gov/idealem/)
   You need to ask access to IDEALEM by giving your information. The whole process from requesting access to getting the tarfile of the code takes less than 10 minutes.

## Clone the test framework

```git clone -b feature/lcrc https://github.com/physnerds/compressiontests.git```

```cd compressiontests```

 Assuming you built and installed all the compression frameworks  and related dependencies, use cmake to build the framework.

Assuming you are in ```tests``` directory
```mkdir build; cd build```

```cmake .. -DZSTD_LIBRARIES=/path/to/compressiontests/SZ3_install/lib64/libzstd.so -DCMAKE_PREFIX_PATH=/path/to/compressiontests/SZ3_install/lib64/cmake -Dmgard_DIR=/path/to/compressiontests/MGARD_install/lib64/cmake/mgard -DSZ_INCLUDE_DIRS=/path/to/compressiontests/SZ3_install/include -DSZ_LIBRARIES=/path/to/compressiontests/SZ3_install/lib64/libSZ3c.so -DIDEALEM_INCLUDE_DIRS=/path/to/compressiontests/idealem-package_install/include -DIDEALEM_LIBRARIES=/path/to/compressiontests/idealem-package_install/lib/libidealem_cxx.a```

And compile the code:
```make```

## Test Codes


1. mytest.cxx: A standalone test code that uses SZ3 to compress and decompress the synthetic waveform data, calculate KS value, compression ratio and draw original and decompressed waveform data. 
TODO: Use the compression_interface tool SZ3CompressionTools.cxx to compress and decompress the data instead.

3. mytest_SZ3.cxx: A test code that uses the APIs of the compression framework (based on SZ3) to do the compression and decompression of the synthetic waveform and writes the original and compressed waveform in RNTuple along with the compression parameters.

3. mytest_mgard.cxx: A test code that uses the APIs of the compression framework (based on MGARD) to do compression and decompression of the synthetic waveform data. 
TODO: Check out how plots of original and decompressed data are done in example 3 and try to reimplement it here.

4. mytest_idealem_RNTUPLE.cxx: A test code that compressed and decompressed the synthetic data and writes the compressed data and decompressed data in RNTuple. 
TODO: Use the compression_interface IDEALEMCompressionTools.hxx to do compression and decompression of the data instead.

5. compress_channels_SZ3.cxx: Compress (using SZ3) one channel readout at a time and write the output in RNTuple.

6. decompress_sz3_test.cxx: Decompress (using SZ3) the output of 5 and create histogram from decompressed and original raw data.








