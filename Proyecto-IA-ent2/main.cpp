#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std; 

struct Restriccion
{
    int val1;
    int val2;
    string operacion; 
    int limit; 
};



vector<vector <int>> Read_Var(){ // Las variables son guardas en un vector de vectores, donde hay 2 elementos, uno de ellos es la variable y el otro el dominio asignado
    ifstream inFile;
    inFile.open("/home/ladros/Programs/Proyecto-IA-ent2/CELAR/scen01/VAR.TXT"); // se debe especificar la ruta de Var.txt
    int vari,dom;
    if (!inFile)
    {
        cout << "Fallo abrir archivo"<<endl;
        exit(1);
    }
    vector<vector<int>> variables_list;
    while (inFile >> vari >> dom)
    {   
        vector<int> par = {0,0};
        par[0] = vari; 
        par[1] = dom; 
        variables_list.push_back(par);
    }
    inFile.close();
    return variables_list;
}

vector<vector<int>> Read_Dom(){ //El dominio se guarda en una lista donde cada indice es el dominio al que corresponde y los elementos son un vector con las posibles frecuencias
    ifstream inFil;
    inFil.open("/home/ladros/Programs/Proyecto-IA-ent2/CELAR/scen01/DOM.TXT"); // se debe especificar la ruta de Dom.txt

    if (!inFil)
    {
        cout << "Fallo abrir archivo"<<endl;
        exit(1);
    }
    vector<vector<int>> raw_values;
    while (!inFil.eof())
    {
        int nDom;
        int cantF;
        inFil >> nDom;
        if (inFil.eof()) // Evita la ultima linea vacia de los archivos
    
        {
            break;
        }
        
        inFil >> cantF;
        vector<int> Frec;
        for (int i = 0; i < cantF; i++)
        {
            int temp;
            inFil >> temp;
            Frec.push_back(temp);
        }
        raw_values.push_back(Frec);

    }
    

    inFil.close();
    return raw_values; 
}

vector<struct Restriccion> Read_Rest(){
    ifstream inDat;
    inDat.open("/home/ladros/Programs/Proyecto-IA-ent2/CELAR/scen01/CTR.TXT"); // se debe especificar la ruta de CTR.txt
    if (!inDat)
    {
        cout << "Fallo abrir archivo"<<endl;
        exit(1);
    }

    int val1;
    int val2;
    string ded; 
    string operation;
    int limit; 
    vector<struct Restriccion> Rest;
    struct Restriccion esqueleto;
    while (inDat >> val1 >> val2 >> ded >> operation >> limit)
    {
        esqueleto.val1 = val1;
        esqueleto.val2 = val2;
        esqueleto.limit = limit; 
        esqueleto.operacion = operation;

        Rest.push_back(esqueleto);

    }
    inDat.close();
    return Rest;  
}

bool asignable(int var, int value_frec,vector<vector<int>> Dom_Asig_Var,vector<vector<int>> Dom ){

    int Dom_asig = Dom_Asig_Var[var][1];

    for (int frec_dom: Dom[Dom_asig])
    {
        if (frec_dom == value_frec)  return true;
    }
    return false;
}   

vector<vector<int>> init_sol(vector<int> FrecuenciasOrdenadas,vector<vector<int>> Dom_Asig_Var,vector<vector<int>> Dom){
    int cant_Frecuencias = FrecuenciasOrdenadas.size();
    int cant_Variables = Dom_Asig_Var.size();
    
    vector<vector<int>> Frec_Asignada(cant_Variables,vector<int> (cant_Frecuencias,0)); //Matriz que ve si la frecuencia fue asignada a cierta antena. [#antenas][#frec]


    int value_frec_act;
    bool put_1 = false;

    for (int var_act = 0; var_act < cant_Variables; var_act++)
    {
        for (int j = 0; j < cant_Frecuencias; j++)
        {   
            value_frec_act = FrecuenciasOrdenadas[j];
            if (asignable(var_act,value_frec_act,Dom_Asig_Var,Dom)==true)
            {
                if (put_1 == false)
                {
                    Frec_Asignada[var_act][j] = 1;
                    put_1 = true;
                }
                   
            }
            
        }
        put_1 = false;  
        
    }
    return Frec_Asignada;
}

vector<int> Frec_Orde(vector<vector<int>> Dom){ //Ordena la lista de frecuencias, asi se tiene claro que frecuencia va con que indice en la solucion
    vector<int> Frec;

    for (vector<int> i : Dom)
    {
        for(int l: i)
        {
            Frec.push_back(l);
        }
    }
    
    sort( Frec.begin(), Frec.end() );
    Frec.erase( unique( Frec.begin(), Frec.end() ), Frec.end() );

    return Frec;
}

int func_objetivo(vector<vector<int>> matriz){ //la funcion objetivo entrega la cantidad de frecuencias que fueron asignadas al menos a 1 antena
    int value = 0;
    for(int i = 0; i<matriz[0].size();i++){
        for(vector<int> antena: matriz){
            if( antena[i] == 1 ){
                value += 1;
                break;
            }
        }
    }
    return value;
}



int funcion_evaluacion(vector<vector<int>> matriz, vector<struct Restriccion> Rest,vector<int> FrecuenciasOrdenadas){
    int Objetivo_value = func_objetivo(matriz);
    int RestFails = 0;

    for(struct Restriccion Cuerpo: Rest){ // obtengo la cantidad de restricciones violadas 
        vector <int> antena1 = matriz[Cuerpo.val1 - 1];
        vector <int> antena2 = matriz[Cuerpo.val2 - 1];

        std::vector<int>::iterator pon1 = find(antena1.begin(), antena1.end(), 1);
        std::vector<int>::iterator pon2 = find(antena2.begin(), antena2.end(), 1);

        int indexFrec1 = distance(antena1.begin(),pon1);
        int indexFrec2 = distance(antena2.begin(),pon2);

        int distancia = abs(FrecuenciasOrdenadas[indexFrec1] - FrecuenciasOrdenadas[indexFrec2]);    
    
        if(Cuerpo.operacion == "="){
            if (distancia != Cuerpo.limit) RestFails += 1;
    
        }else if (Cuerpo.operacion == ">")
        {
            if(distancia < Cuerpo.limit) RestFails += 1;
        }
    }

    int final = Objetivo_value + 4*RestFails;
    return final;
}

int SimulatedAnneling(vector<vector<int>> Sol_Inicial){
    int Temp_init = 100; //dejamos temperatura inicial 100 
    int IterMax = 100;
}

int main(){
    
    vector<vector<int>> Dom_Asig_Var = Read_Var();
    vector<vector<int>> Dom = Read_Dom();
    vector<struct Restriccion> Rest = Read_Rest();
    vector<int> FrecuenciasOrdenadas = Frec_Orde(Dom);
    vector<vector<int>> sol_inicial = init_sol(FrecuenciasOrdenadas,Dom_Asig_Var,Dom);
    cout<<funcion_evaluacion(sol_inicial,Rest,FrecuenciasOrdenadas)<<endl;
    return 0;
}