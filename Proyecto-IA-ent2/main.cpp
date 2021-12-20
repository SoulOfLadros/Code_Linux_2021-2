#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include<time.h>
#include <cmath>

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
   

vector<vector<int>> init_sol(vector<int> FrecuenciasOrdenadas,vector<vector<int>> Dom_Asig_Var,vector<vector<int>> Dom){
    int cant_Frecuencias = FrecuenciasOrdenadas.size();
    int cant_Variables = Dom_Asig_Var.size();
    
    vector<vector<int>> Frec_Asignada(cant_Variables,vector<int> (cant_Frecuencias,0)); //Matriz que ve si la frecuencia fue asignada a cierta antena. [#antenas][#frec]
    
    srand(time(0));

    for(int n_antena = 0; n_antena < cant_Variables; n_antena++){
        int dom_index_var = Dom_Asig_Var[n_antena][1];
        int eleccion = rand() % Dom[dom_index_var].size(); 

        std::vector<int>::iterator lugar = find(Dom[dom_index_var].begin(), Dom[dom_index_var].end(), Dom[dom_index_var][eleccion]);
        int indexFrec1 = distance(Dom[dom_index_var].begin(),lugar);

        Frec_Asignada[n_antena][indexFrec1] = 1;
    }

    cout<<"[";
    for(int elem: FrecuenciasOrdenadas){
        cout<< elem<< " ";
    }
    cout<<"]"<<endl;


    for(vector<int> line: Frec_Asignada){
        cout<<"[";
        for(int i: line){
            cout<<i<<" ";
        }
        cout<<"]"<<endl;
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


vector<int> posibles_cambios(int index_antena,int index_frecAsig,vector<vector<int>> Dom_Asig_Var,vector<vector<int>> Dom ,vector<int> FrecuenciasOrdenadas){

    vector<int> asignacion = Dom[Dom_Asig_Var[index_antena][1]];

    vector<int> index_cambios; 
    for(int values: asignacion){
        std::vector<int>::iterator pon = find(FrecuenciasOrdenadas.begin(), FrecuenciasOrdenadas.end(), values);
        int index_value = distance(FrecuenciasOrdenadas.begin(),pon);
        if(index_frecAsig == index_value) continue;
        else
        {
            index_cambios.push_back(index_value);
        }
        
    }
    return index_cambios;
}

vector<vector<int>> SimulatedAnneling(){
    vector<vector<int>> Dom_Asig_Var = Read_Var();
    vector<vector<int>> Dom = Read_Dom();
    vector<struct Restriccion> Rest = Read_Rest();
    vector<int> FrecuenciasOrdenadas = Frec_Orde(Dom);
    vector<vector<int>> sol_act = init_sol(FrecuenciasOrdenadas,Dom_Asig_Var,Dom);
    
    float T = 100.0;
    int iter_max = 100; 
    
   
    int iter = 0;
    
    int eval_act = funcion_evaluacion(sol_act,Rest,FrecuenciasOrdenadas);
    
    while (T != 0 && iter != iter_max)
    {
        bool newsol = false; 
        srand(time(0));
        vector<int> mov_antenas; //lista de indices de las antenas a revisar de forma aleatoria.
        for(int o = 0; 0<sol_act.size();o++){
            mov_antenas.push_back(o);
        }
        random_shuffle(mov_antenas.begin(),mov_antenas.end());

        for(int antena: mov_antenas){
            //revisar los movimientos de tal antena 
            vector<int> ant_act = sol_act[antena];
            std::vector<int>::iterator poin = find(ant_act.begin(), ant_act.end(), 1);
            int index_value = distance(ant_act.begin(),poin);

            vector <int> spwas = posibles_cambios(antena,index_value,Dom_Asig_Var,Dom,FrecuenciasOrdenadas);
            
            random_shuffle(spwas.begin(),spwas.end());
            for(int index_posible: spwas){ // revisar el candidado de frecuencia
                vector<vector<int>> nueva_sol = sol_act;
                nueva_sol[antena][index_value] = 0;
                nueva_sol[antena][index_posible] = 1;

                int nueva_sol_value = funcion_evaluacion(nueva_sol,Rest,FrecuenciasOrdenadas);

                int variation = eval_act - nueva_sol_value; //actual - nueva porque estamos minizando.
                float random01 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                if(variation > 0){
                    sol_act = nueva_sol;
                    eval_act = nueva_sol_value;
                    newsol = true;
                    break;
                } else if ( exp(variation/T) > random01)
                {
                    sol_act = nueva_sol;
                    eval_act = nueva_sol_value;
                    newsol = true;
                    break;
                }
            }
            if(newsol == true){
                T=T*0.9;
                iter= iter +1 ;
                break;
            }
        }
        if(newsol == false){
            return sol_act;
        }
    }
    return sol_act; 
}





int main(){
    vector<vector<int>> resultado = SimulatedAnneling();
    cout<<"------------------------------------------------------------------------------------"<<endl;
    for(vector<int> line: resultado){
        cout<<"[";
        for(int i: line){
            cout<<i<<" ";
        }
        cout<<"]"<<endl;
    }
}