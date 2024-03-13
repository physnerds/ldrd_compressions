import numpy as np
import random,h5py

#python class that generates waveform fake-data 
# number of waves and number of data sets as user defined parameters.
# wave properties (frequencies, amplitudes, phase shifts) are randomized.

class waveforms:
    def __init__(self,hfile, n_data, n_waves):
        self.hfile = hfile
        self.n_data = n_data
        self.n_waves = n_waves
  
    ###############################################################
     
    def create_waveform(self,frequencies,amplitudes,phases):
        sampling_rate = 100000 #how often to record the DAQ
        duration = 2 #Duration in seconds...
        #the time values...
        time_val = np.linspace(0,duration, int(sampling_rate*duration),endpoint=False)

        y_tot = np.zeros_like(time_val)
        for freq, amp, phase in zip(frequencies,amplitudes,phases):
            y = amp*np.sin(2*np.pi*freq*time_val+phase)
            y_tot += y 
        return y_tot
    ###############################################################
    #we generate parameters for the waveform and return the wave....

    def generate_parameters(self):
        #Create frequencies for third to n_waves wave
        t_rand_freq = np.random.randint(3,self.n_waves,size=(1,self.n_waves-2))
        sum_freq = np.sum(t_rand_freq)
        # we want to add 2 major waves...
        frequencies = [1,2] + t_rand_freq.flatten().tolist()

        avail_phases = [np.pi/2,3*np.pi/2,0,np.pi/4]
        phases = []
        for i in range(0,self.n_waves-2):
            phases.append(random.choice(avail_phases))
        phases[0:0] = [3*np.pi/4, np.pi]

        #now the amplitude...
        amplitudes = []
        for freq in t_rand_freq.flatten():
            amplitudes.append(float(freq/sum_freq)) #make it double precision
        #I want to add two major amplitudes that defines tha shape of the 
        #wave form....
        amplitudes[0:0] = [1.0,0.8] 

        return self.create_waveform(frequencies,amplitudes,phases)
    ###############################################################
    #write data into hdf5 file... 
    def write_datasets(self):
        for i in range(0,self.n_data):
            data = self.generate_parameters()
            dset_name = "waveform_"+str(i)
            self.hfile.create_dataset(dset_name,data=data)
        

    ###############################################################
    #now the reading part....
    def read_dataset(self,dsetpath):
        dataset = self.hfile[dsetpath]
        return dataset[:]
    #########################################################################
    #Iteratively go through the hdf5 data-sets and print compression info.
    def get_compressionInfo(self,dsetpath):
        with self.hfile as file:
            def print_compression(name,obj):
                if isinstance(obj, h5py.Dataset):
                    print(f" DataSet: {name}")
                    print(f" Compression: {obj.compression}")
                    print(f" Compression opts: {obj.compression_opts}")
            file.visititems(print_compression)
    #########################################################################
    