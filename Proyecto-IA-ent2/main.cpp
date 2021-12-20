#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include<time.h>
#include <cmath>



struct Restriccion
{
    int val1;
    int val2;
    std::string operacion; 
    int limit; 
};



std::vector<std::vector <int>> Read_Var(){ // Las variables son guardas en un vector de vectores, donde hay 2 elementos, uno de ellos es la variable y el otro el dominio asignado
    std::ifstream inFile;
    std::string nameFile;
    std::cout<< "Ingrese Nombre del archivo de variables (ejemplo VAR1.TXT): ";
    std::cin>>nameFile;
    inFile.open(nameFile); // se debe especificar la ruta de Var.txt
    int vari,dom;
    if (!inFile)
    {
        std::cout << "Fallo abrir archivo"<<std::endl;
        exit(1);
    }
    int tru_index = 0;
    std::vector<std::vector<int>> variables_list;
    while (inFile >> vari >> dom)
    {   
        std::vector<int> par = {0,0,0};
        par[0] = vari; 
        par[1] = dom;
        par[2] = tru_index; 
        variables_list.push_back(par);
        tru_index+=1;
    }
    inFile.close();
    return variables_list;
}

std::vector<std::vector<int>> Read_Dom(){ //El dominio se guarda en una lista donde cada indice es el dominio al que corresponde y los elementos son un vector con las posibles frecuencias
    std::ifstream inFil;
    std::string nameFile;
    std::cout<< "Ingrese Nombre del archivo de dominio (ejemplo DOM1.TXT): ";
    std::cin>>nameFile;
    inFil.open(nameFile); // se debe especificar la ruta de Dom.txt

    if (!inFil)
    {
        std::cout << "Fallo abrir archivo"<<std::endl;
        exit(1);
    }
    std::vector<std::vector<int>> raw_values;
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
        std::vector<int> Frec;
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

std::vector<struct Restriccion> Read_Rest(){
    std::ifstream inDat;
    std::string nameFile;
    std::cout<< "Ingrese Nombre del archivo de restricciones (ejemplo CTR1.TXT): ";
    std::cin>>nameFile;
    inDat.open(nameFile); // se debe especificar la ruta de CTR.txt
    if (!inDat)
    {
        std::cout << "Fallo abrir archivo"<<std::endl;
        exit(1);
    }

    int val1;
    int val2;
    std::string ded; 
    std::string operation;
    int limit; 
    std::vector<struct Restriccion> Rest;
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
   

std::vector<std::vector<int>> init_sol(std::vector<int> FrecuenciasOrdenadas,std::vector<std::vector<int>> Dom_Asig_Var,std::vector<std::vector<int>> Dom){
    int cant_Frecuencias = FrecuenciasOrdenadas.size();
    int cant_Variables = Dom_Asig_Var.size();
    
    std::vector<std::vector<int>> Frec_Asignada(cant_Variables,std::vector<int> (cant_Frecuencias,0)); //Matriz que ve si la frecuencia fue asignada a cierta antena. [#antenas][#frec]
    
    srand(time(0));

    for(int n_antena = 0; n_antena < cant_Variables; n_antena++){
        int dom_index_var = Dom_Asig_Var[n_antena][1];
        int eleccion = rand() % Dom[dom_index_var].size(); 

        std::vector<int>::iterator lugar = find(FrecuenciasOrdenadas.begin(), FrecuenciasOrdenadas.end(), eleccion);
        int indexFrec1 = distance(FrecuenciasOrdenadas.begin(),lugar);

        Frec_Asignada[n_antena][indexFrec1] = 1;
    }


    // for(vector<int> line: Frec_Asignada){
    //     cout<<"[";
    //     for(int i: line){
    //         cout<<i<<" ";
    //     }
    //     cout<<"]"<<endl;
    // }

    return Frec_Asignada;
}

std::vector<int> Frec_Orde(std::vector<std::vector<int>> Dom){ //Ordena la lista de frecuencias, asi se tiene claro que frecuencia va con que indice en la solucion
    std::vector<int> Frec;

    for (std::vector<int> i : Dom)
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

int func_objetivo(std::vector<std::vector<int>> matriz){ //la funcion objetivo entrega la cantidad de frecuencias que fueron asignadas al menos a 1 antena
    int value = 0;
    for(int i = 0; i<matriz[0].size();i++){
        for(std::vector<int> antena: matriz){
            if( antena[i] == 1 ){
                value += 1;
                break;
            }
        }
    }
    return value;
}



int funcion_evaluacion(std::vector<std::vector<int>> matriz, std::vector<struct Restriccion> Rest,std::vector<int> FrecuenciasOrdenadas,std::vector<std::vector<int>> Dom_Asig_Var){
    int Objetivo_value = func_objetivo(matriz);
    int RestFails = 0;

    for(struct Restriccion Cuerpo: Rest){ // obtengo la cantidad de restricciones violadas 
        std::vector <int> antena1;
        std::vector <int> antena2;
        for(std::vector<int> datos: Dom_Asig_Var){
            if(datos[0] == Cuerpo.val1){
                antena1 = matriz[datos[2]];
            }
        }
        for(std::vector<int> datos: Dom_Asig_Var){
            if(datos[0] == Cuerpo.val2){
                antena2 = matriz[datos[2]];
            }
        }
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


std::vector<int> posibles_cambios(int index_antena,int index_frecAsig,std::vector<std::vector<int>> Dom_Asig_Var,std::vector<std::vector<int>> Dom ,std::vector<int> FrecuenciasOrdenadas){

    std::vector<int> asignacion = Dom[Dom_Asig_Var[index_antena][1]];

    std::vector<int> index_cambios; 
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

std::vector<std::vector<int>> SimulatedAnneling(float T,int iter_max,float factor){
    std::vector<std::vector<int>> Dom_Asig_Var = Read_Var();
    std::vector<std::vector<int>> Dom = Read_Dom();
    std::vector<struct Restriccion> Rest = Read_Rest();
    std::vector<int> FrecuenciasOrdenadas = Frec_Orde(Dom);
    std::vector<std::vector<int>> sol_act = init_sol(FrecuenciasOrdenadas,Dom_Asig_Var,Dom);
    
   
    int iter = 0;
    
    int eval_act = funcion_evaluacion(sol_act,Rest,FrecuenciasOrdenadas,Dom_Asig_Var);
    srand(time(0));
    std::cout<<"Eval_inicial: "<<eval_act<<std::endl;
    std::cout<<"Factor de disminucion: "<<factor<<std::endl;
    while (T != 0.0 && iter != iter_max)
    {
        bool newsol = false; 
        std::vector<int> mov_antenas; //lista de indices de las antenas a revisar de forma aleatoria.
        for(int o = 0; o<sol_act.size();o++){
            mov_antenas.push_back(o);
        }
        random_shuffle(mov_antenas.begin(),mov_antenas.end());
        for(int antena: mov_antenas){
            //revisar los movimientos de tal antena 
            std::vector<int> ant_act = sol_act[antena];
            std::vector<int>::iterator poin = find(ant_act.begin(), ant_act.end(), 1);
            int index_value = distance(ant_act.begin(),poin);
            std::vector <int> spwas = posibles_cambios(antena,index_value,Dom_Asig_Var,Dom,FrecuenciasOrdenadas);
            
            random_shuffle(spwas.begin(),spwas.end());

            for(int index_posible: spwas){ // revisar el candidado de frecuencia
                
                std::vector<std::vector<int>> nueva_sol = sol_act;
                nueva_sol[antena][index_value] = 0;
                nueva_sol[antena][index_posible] = 1;

                int nueva_sol_value = funcion_evaluacion(nueva_sol,Rest,FrecuenciasOrdenadas,Dom_Asig_Var);

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
                T=T*factor;
                iter= iter +1 ;
                break;
            }
        }
        if(newsol == false){
            std::cout<<"Temperatura: "<<T<<std::endl;
            std::cout<<"Iteracion: "<<iter<<std::endl;
            std::cout<<"EvalFinal: "<<eval_act<<std::endl;
            return sol_act;
        }
    }
    std::cout<<"Temperatura: "<<T<<std::endl;
    std::cout<<"Iteracion: "<<iter<<std::endl;
    std::cout<<"EvalFinal: "<<eval_act<<std::endl;
    return sol_act; 
}





int main(){
    float Temperatura;
    int IteracionesMax; 
    float FactorReduccion;
    std::cout<<"Ingrese Temperatura inicial: ";
    std::cin>>Temperatura;
    std::cout<<"Ingrese cantidad maxima de iteraciones: ";
    std::cin>>IteracionesMax;
    std::cout<<"Ingrese factor de disminucion de la temperatura: ";
    std::cin>>FactorReduccion;

    std::vector<std::vector<int>> resultado = SimulatedAnneling(Temperatura,IteracionesMax,FactorReduccion);
    // cout<<"------------------------------------------------------------------------------------"<<endl;
    // for(vector<int> line: resultado){
    //     cout<<"[";
    //     for(int i: line){
    //         cout<<i<<" ";
    //     }
    //     cout<<"]"<<endl;
    // }
}