from waveforms import waveforms
import h5py,os,sys
import matplotlib.pyplot as plt

filename = "waveforms.h5"
n_data = 500 #how many datasets do we want...
n_waves = 7 #how many waves do we want....
if int(sys.argv[1])==1:   
    hf = h5py.File(filename,'w')
    wf = waveforms(hf,n_data,n_waves)
    wf.write_datasets()
elif int(sys.argv[1])==2:
    if not os.path.exists(filename):
        print("File ",filename," does not exist")
        sys.exit()
    hf = h5py.File(filename,'r')
    wf = waveforms(hf,n_data,n_waves)
    dataset  = wf.read_dataset("waveform_1")
    wf.get_compressionInfo("waveform_1")
    plt.plot(dataset)
    plt.xlabel("Time Axis")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.savefig("Waveform_1.png",dpi=300)
else:
    print("sys.argv ",sys.argv)
    print("1 for Write and 2 for Read")
