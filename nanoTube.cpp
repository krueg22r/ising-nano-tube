//Program to simulate the behavior of a 1-d lattice in a tube
//Hard coded: 5 sites, boundary made up of fixed +1 spins
//Taken as input: parameters for interaction btwn. spins,
//interaction between spin and tube, how many steps, output file name

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <time.h>
using namespace std; 

int getN(int spins[]){
        int counter = 0;
        for(int i = 0; i < 5; i++){
                counter += (spins[i]+1)/2;
        }
        return counter;
}

double getEn(int spins[], double J, double epsilon){
	double nnTerms = 0; 
	for(int i = 0; i < 4; i++){
		nnTerms += -1 * J * spins[i] * spins[i+1]; 
	} 
	double tubeTerms = (2 * getN(spins) - 5) * 4 * epsilon; 
	double ends = -1 * J * spins[0] + -1 * J * spins[4];
	return (nnTerms + tubeTerms + ends);  
	
}

int getInt(int spins[]){
	int counter = 0; 
	for(int i = 0; i < 4; i++){
		if(spins[i] != spins[i+1]){
			 counter++; 
		}
	}
	if(spins[0] == -1) counter++; 
	if(spins[4] == -1) counter++; 
	return counter; 
}

double trialMove(int spinsO[], int spinsN[], double J, double epsilon, double energyO){
	//randomly chose a spin (0-4) 
	int tagSpin = rand() % 5; 
	for(int i = 0; i < 5; i++) spinsN[i] = spinsO[i]; 
	spinsN[tagSpin] = -1 * spinsN[tagSpin]; 
	double energyN = getEn(spinsN, J, epsilon); 
	double de = energyN - energyO; 
	double r = (double)rand() / (double)RAND_MAX;
	if(r < (exp(-1*de))){
		spinsO[tagSpin] = (spinsO[tagSpin]*-1); //accept trial move 
		energyO = energyN; 
	} //otherwise don't update
	return energyO; 
}

int main(){
	double J; //J and epsilon are given in units of kbT
	double epsilon; 
	int steps; 
	string outFile;  
	srand(time(NULL)); //seed random number generator using system clock
	//read in run parameters
	cout <<"enter J (units of kbT): \n";
	cin >> J; 
	cout <<"enter epsilon (units of kbT): \n";
	cin >> epsilon;  
	cout <<  "enter number of MC steps for simulation: \n"; 
	cin >> steps; 
	cout <<"enter name of configuration data output file: \n"; 
	cin >>outFile;
	ofstream traj; 
	traj.open(outFile.c_str()); 
	ofstream prob; 
	string pFileName = "probs_"+outFile;  
	prob.open(pFileName.c_str()); 
	ofstream debug; 
	debug.open("debug.out"); 
	ofstream energies; 
	double t = 0; 
	int step = 0;
	// data structure for old spins (accepted at last step) and new
	int spinsO[5];
	int spinsN[5];  
	int hist[6]; 
	for(int i = 0; i < 5; i++){
		 spinsO[i] = -1; 
		 spinsN[i] = -1; 
		 hist[i] = 0; 
	}
	hist[5] = 0; 
	//initialize with random spins
	//if for loop below is commented out, we will begin with an empty tube
	/*for(int i = 0; i < 5; i++){
		int r = rand() % 2;
		if(r==1){
			spinsO[i] = 1; 	
		}else{
			spinsO[i] = -1; 
		}
	}*/
	double energy = getEn(spinsO, J, epsilon); 
	int N = getN(spinsO); 
	int interfaces = getInt(spinsO);
	int totN = 0;
	traj <<"# Step     N	Interfaces      Configuration"<<endl;   
	//enter MC loop 
	t = 0; 
	do{
		//first write to file
		traj<<setw(7)<<t<<setw(7)<<N<<setw(7)<<interfaces<<"        "; 
		for(int i = 0; i < 5; i++){
                        traj << spinsO[i]<<" "; 
                }
                traj<<endl; 
		hist[N]++;
		//then take next step
		t++; 
		double eNew = trialMove(spinsO,spinsN,J,epsilon,energy);
		energy = eNew;
		N = getN(spinsO);
		interfaces = getInt(spinsO);
		totN += N;  
	}while (t < steps);//end MC loop
	traj << "# Average N = "<<(double)totN/steps<<endl;  
	for(int i = 0; i < 6; i++){
		prob<< setw(14) << i << setw(14) << hist[i]/(double)steps <<endl;  
	}
	return 0; 	
}
