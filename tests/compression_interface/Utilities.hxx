#ifndef UTILITIES_HXX
#define UTILITIES_HXX

#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TAxis.h"

template<typename T>
std::vector<T> ReturnH5Data(H5::H5File file, std::string dsetname){
    
    H5::DataSet dset = file.openDataSet(dsetname);
    H5::DataType dtype = dset.getDataType();
    H5::DataSpace dspace = dset.getSpace();

    int ndims = dspace.getSimpleExtentNdims();
    hsize_t dims_out[ndims];
    dspace.getSimpleExtentDims(dims_out,NULL);

    std::vector<T>out_data(dims_out[0]);

    dset.read(out_data.data(),H5::PredType::NATIVE_DOUBLE);

    return out_data;
}

//writes a 1-D vector into CSV 
template<typename T>
void WriteCSVFile(std::vector<T>in_data,char* csv_fname){
    std::ofstream outFile(csv_fname);
    for(size_t i = 0;i<in_data.size();i++){
        outFile<<in_data[i]<<"\n";
    }
    outFile.close();
}

//calculate KS Test Using ROOT
template<typename T>
double CalculateKSVal(size_t out_size, T* orig_data,T* decompressed_data ){
    auto test = ROOT::Math::GoFTest(out_size, orig_data, out_size, decompressed_data);
    double KSVal = test.KolmogorovSmirnov2SamplesTest();
    return KSVal;
}

//writes from CSV into a 1D vector...
template<typename T>
std::vector<T>ReadCSVFile(std::string& fname){
    std::ifstream file(fname);
    std::vector<T> data;
    if(!file.is_open())
        throw std::runtime_error("Cannot open the File");
    
    std::string line;
    while(std::getline(file,line)){
        std::stringstream lineStream(line);
        std::string cell;

        while(std::getline(lineStream,cell,',')){
            cell.erase(remove_if(cell.begin(),cell.end(),isspace), cell.end());
            try {
                T val = std::stod(cell);
                data.push_back(val);
            } catch (const std::invalid_argument& ia){
                std::cerr<<" Invalid Argument "<<ia.what()<<"\n";
                continue;
            } catch (const std::out_of_range& oor){
                std::cerr<<" Out of Range "<<oor.what()<<"\n";
                continue;
            }

        }
    }

    file.close();
    return data;
}

void DrawGraphs(double* orig_data, double* decomp_data, size_t dat_size,std::string cfname, double ksVal, int comp_ratio, bool diff=false){
    auto gr_orig = new TGraph(dat_size,orig_data);
    auto gr_decomp = new TGraph(dat_size,decomp_data);
    std::vector<double> diff_val{};
    std::vector<double> div_val{};
    for(int i =0;i<dat_size;i++){
        diff_val.push_back(orig_data[i]-decomp_data[i]);
        if(decomp_data[i]!=0)
            div_val.push_back(orig_data[i]/decomp_data[i]);
        else
            div_val.push_back(-999);
    }

    auto gr_diff = new TGraph(dat_size,diff_val.data());
    auto gr_div = new TGraph(dat_size,div_val.data());
    gr_decomp->SetLineColor(kRed);
    gr_diff->SetLineColor(kRed);
    gr_div->SetLineColor(kRed);
    gr_orig->SetLineWidth(3);
    gr_diff->SetLineWidth(3);
    gr_div->SetLineWidth(3);
    gr_decomp->SetLineWidth(2);
    //gr_decomp->SetLineStyle(10);
    gr_orig->SetTitle("Original");
    gr_decomp->SetTitle("Decomposed");
    gr_diff->SetTitle("Difference");
    gr_div->SetTitle("Ratio of Original to Decomposed");

    auto mg = new TMultiGraph();
    std::string mg_title = "KS-VAL: "+std::to_string(ksVal)+", CR: "+std::to_string(comp_ratio);
    mg->Add(gr_orig,"l");
    //mg->Add(gr_diff,"l");
    mg->Add(gr_decomp,"l");
    mg->SetTitle(mg_title.c_str());

    auto c = new TCanvas("c","c",750,750);
    auto pad1 = new TPad("pad1","Waveform Overlayed",0.05,0.25,0.95,0.95);
    TPad* pad2;
    if(diff)
        pad2 = new TPad("pad2", "Waveform Difference",0.05,0.05,0.95,0.25);
    else
        pad2 = new TPad("pad2", "Waveform Divided",0.05,0.05,0.95,0.25);
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    mg->Draw("a");
    pad1->Update();
    pad2->cd();
    if(diff) 
        gr_diff->Draw();
    else 
        gr_div->Draw();
    //c->SetLogx();
    pad2->Update();
    //c->BuildLegend();
    c->SaveAs(cfname.c_str());
    //c->SaveAs((cfname+".root").c_str());
}

std::string readJSONFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeJSON(nlohmann::json jobj,std::string f_name){
    std::string j_data = jobj.dump(4);
    std::ofstream outfile(f_name.c_str());
    outfile<<j_data;
    outfile.close();
}

#endif
