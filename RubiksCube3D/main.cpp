// Autor: Nedeljko Tesanovic
// Opis: Zestoko iskomentarisan program koji crta sareni trougao u OpenGL-u

#define _CRT_SECURE_NO_WARNINGS
 //Biblioteke za stvari iz C++-a (unos, ispis, fajlovi itd) 
#include <iostream>
#include <fstream>
#include <sstream>

//Biblioteke OpenGL-a
#include <GL/glew.h>   //Omogucava upotrebu OpenGL naredbi
#include <GLFW/glfw3.h>//Olaksava pravljenje i otvaranje prozora (konteksta) sa OpenGL sadrzajem

unsigned int compileShader(GLenum type, const char* source); //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
unsigned int createShader(const char* vsSource, const char* fsSource); //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource i Fragment sejdera na putanji fsSource

int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Pokretanje GLFW biblioteke
    // Nju koristimo za stvaranje okvira prozora / konteksta
    if (!glfwInit()) // !0 == 1  | glfwInit inicijalizuje GLFW i vrati 1 ako je inicijalizovana uspjesno, a 0 ako nije
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    //Odredjivanje OpenGL verzije i profila (3.3, programabilni pajplajn)
    //bez ovoga, koristi se najnoviji moguci OpenGL koji hardver podrzava
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Stvaranje prozora
    GLFWwindow* window; //Mjesto u memoriji za prozor
    unsigned int wWidth = 500;
    unsigned int wHeight = 500;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); // Napravi novi prozor
    // glfwCreateWindow( sirina, visina, naslov, monitor na koji ovaj prozor ide preko citavog ekrana (u tom slucaju umjesto NULL ide glfwGetPrimaryMonitor() ), i prozori sa kojima ce dijeliti resurse )
    if (window == NULL) //Ako prozor nije napravljen
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); //Gasi GLFW
        return 2; //Vrati kod za gresku
    }
    // Postavljanje novopecenog prozora kao aktivni (sa kojim cemo da radimo)
    glfwMakeContextCurrent(window);

    // Inicijalizacija GLEW biblioteke
    if (glewInit() != GLEW_OK) //Slicno kao glfwInit. GLEW_OK je predefinisani kod za uspjesnu inicijalizaciju sadrzan unutar biblioteke
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int basicShader = createShader("basic.vert", "basic.frag"); // Napravi objedinjeni sejder program

    float vertices[] = //Tjemena trougla koja sadrze sve informacije o njemu. Mi definisemo podatke u onom formatu koju mi zelimo
    {
        //Podaci su poredani za nasu citkivost - racunar ne vidi ni razmake ni redove.
        //Moramo mu naknadno reci kako da intepretira ove podatke

        //Pozicija    |    Boja = RGBA (R - Crvena, G - Zelena, B - Plava, A = neprovidno; Opseg od 0 do 1, gdje je 0 crno a 1 svijetlo)
        //X    Y       R    G    B    A
        0.25, 0.0,    1.0, 0.0, 0.0, 1.0, //prvo tjeme, crveno
       -0.25, 0.0,    0.0, 0.0, 1.0, 1.0, //drugo tjeme, plavo
        0.0 ,-0.5,    1.0, 1.0, 1.0, 1.0 //trece tjeme, bijelo
    };
    unsigned int stride = (2 + 4) * sizeof(float); //Korak pri kretanju po podacima o tjemenima = Koliko mjesta u memoriji izmedju istih komponenti susjednih tjemena
    //U nasem slucaju XY (2) + RGBA (4) = 6
    //Za svrhe vjezbi ovo je efektivno velicina jednog tjemena

    //Vertex Array Object - Sadrzi bar 16 pokazivaca na atribute koji opusuju sadrzaje bafer objekata
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); //Generisi 1 VAO na adresi datoj adresi
    glBindVertexArray(VAO); //Povezi VAO za aktivni kontekst - sva naknadna podesavanja ce se odnositi na taj VAO

    //Vertex Buffer Object - Bafer objekat, u ovom slucaju za tjemena trougla koji crtamo
    unsigned int VBO;
    glGenBuffers(1, &VBO); //Generisi 1 bafer na datoj adresi 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Povezi dati bafer za aktivni kontekst. Array buffer se koristi za tjemena figura.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Slanje podataka na memoriju graficke karte
    //glBufferData(koji bafer, koliko podataka ima, adresa podataka, nacin upotrebe podataka (GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW;)

    //Opisivanje pokazivaca na atribute: Pokazivac 0 ceo opisati poziciju (koordinate x i y), a pokazivac 1 boju (komponente r, g, b i a).
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0); 
    //glVertexAttribPointer(indeks pokazivaca, broj komponenti atributa, tip komponenti atributa, da li je potrebno normalizovati podatke (nama uvijek GL_FALSE), korak/velicina tjemena, pomjeraj sa pocetka jednog tjemena do komponente za ovaj atribut - mora biti (void*))  
    glEnableVertexAttribArray(0); //Aktiviraj taj pokazivac i tako intepretiraj podatke
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); //Objasni da su preostala cetiri broja boja (preskacemo preko XY od pozicije, pa je pomjeraj 2 * sizeof(float))
    glEnableVertexAttribArray(1);

    //Postavili smo sta treba, pa te stvari iskljucujemo, da se naknadna podesavanja ne bi odnosila na njih i nesto poremetila
    //To radimo tako sto bindujemo 0, pa kada treba da nacrtamo nase stvari, samo ih ponovo bindujemo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    glClearColor(0.15, 0.15, 0.15, 1.0); //Podesavanje boje pozadine (RGBA format);

    while (!glfwWindowShouldClose(window)) //Beskonacna petlja iz koje izlazimo tek kada prozor treba da se zatvori
    {
        //Unos od korisnika bez callback funckcije. GLFW_PRESS = Dugme je trenutno pritisnuto. GLFW_RELEASE = Dugme trenutno nije pritisnuto
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        //Brisanje ekrana
        glClear(GL_COLOR_BUFFER_BIT);

        // [KOD ZA CRTANJE]
        glUseProgram(basicShader); //Izaberi nas sejder program za crtanje i koristi ga za svo naknadno crtanje (Ukoliko ne aktiviramo neke druge sejder programe)
        glBindVertexArray(VAO); //Izaberemo sta zelimo da crtamo
        glDrawArrays(GL_TRIANGLES, 0, 3); //To i nacrtamo
        //glDrawArrays(tip primitive, indeks pocetnog tjemena, koliko narednih tjemena crtamo);
        glBindVertexArray(0);
        glUseProgram(0);

        //Zamjena vidljivog bafera sa pozadinskim
        glfwSwapBuffers(window);

        //Hvatanje dogadjaja koji se ticu okvira prozora (promjena velicine, pomjeranje itd)
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++


    //Brisanje bafera i sejdera
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(basicShader);
    //Sve OK - batali program
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
