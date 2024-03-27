#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"

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

void DrawGraphs(double* orig_data, double* decomp_data, size_t dat_size,std::string cfname, double ksVal, int comp_ratio){
    auto gr_orig = new TGraph(dat_size,orig_data);
    auto gr_decomp = new TGraph(dat_size,decomp_data);
    std::vector<double> diff_val;
    for(int i =0;i<dat_size;i++)diff_val.push_back(orig_data[i]-decomp_data[i]);

    auto gr_diff = new TGraph(dat_size,diff_val.data());

    gr_decomp->SetLineColor(kBlue);
    gr_diff->SetLineColor(kRed);
    gr_orig->SetLineWidth(3);
    gr_diff->SetLineWidth(3);
    gr_decomp->SetLineWidth(3);
    gr_orig->SetTitle("Original");
    gr_decomp->SetTitle("Decomposed");
    gr_diff->SetTitle("Difference");

    auto mg = new TMultiGraph();
    std::string mg_title = "KS-VAL: "+std::to_string(ksVal)+", CR: "+std::to_string(comp_ratio);
    //mg->Add(gr_orig,"l");
    mg->Add(gr_diff,"l");
    //mg->Add(gr_decomp,"l");
    mg->SetTitle(mg_title.c_str());

    auto c = new TCanvas("c","c",750,750);
    c->cd();
    mg->Draw("a");
    c->BuildLegend();
    c->SaveAs(cfname.c_str());
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