from waveforms import waveforms
import h5py,os,sys,random 
import matplotlib.pyplot as plt

filename = "waveforms.h5"
n_data = 500 #how many datasets do we want...
n_waves = 7 #how many waves do we want....
if int(sys.argv[1])==1:   
    hf = h5py.File(filename,'w')
    wf = waveforms(hf,n_data,n_waves)
    signal_strength = 10 #Strength of the signal event (spike)
    start_idx = 100000 #Where does the signal start in time stamp
    dset_start_idx = 300 #which channel it appears first
    dset_end_idx = 400 #which channel it appears last
    wf.write_datasetsWithSignal(signal_strength,start_idx,dset_start_idx,dset_end_idx)
elif int(sys.argv[1])==2:
    if not os.path.exists(filename):
        print("File ",filename," does not exist")
        sys.exit()
    hf = h5py.File(filename,'r')
    wf = waveforms(hf,n_data,n_waves)
    dataset  = wf.read_dataset("waveform_300")
    wf.get_compressionInfo("waveform_300")
    plt.plot(dataset)
    plt.xlabel("Time Axis")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.savefig("Waveform_300.png",dpi=300)
else:
    print("sys.argv ",sys.argv)
    print("1 for Write and 2 for Read")
