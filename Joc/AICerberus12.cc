#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Cerberus12      // moviments amb prioritats vaig a per menjar o zombie mes proper, i ataco si tinc adjacent enemic, despres zombie, despres menjar.
                                    // Escapar zombies diagonals, enemics mirar probabilitat de win al atacar i si perdo escapar (tots casos mirats) i canvi prob winrate


struct PLAYER_NAME : public Player {

    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
    * Types and attributes
    */

    // CONSTANTS -------------------------------------------------------------------------------
    const vector<Dir> dirs = {Left, Right, Down, Up};
    const vector<Dir> diagonals = {UL, RU, DR, LD};


    // TYPEDEFS -------------------------------------------------------------------------------
    typedef vector<int> VI;
    typedef vector<VI> VVI;
    typedef vector<bool> VB;
    typedef vector<VB> VVB;
    typedef vector<Dir> VD;
    typedef vector<VD> VVD;
    typedef vector<Pos> VP;


    // FUNCIONS -------------------------------------------------------------------------------

    // True si no te Waste la pos
    bool sense_waste(const Pos& p) const { return cell(p).type != Waste; }


    // True si no hi ha una unitat meva a la pos
    bool sense_u_meva(const Pos& p) const {
        int id = cell(p).id;

        if (id != -1) {  // si hi ha una tropa
            if (unit(id).player == me()) return false;
        }
        return true;
    }


    // True si no hi ha unitat morta a la pos
    bool sense_u_morta(const Pos& p) const {
        int id = cell(p).id;
        if (id != -1) {
            if (unit(id).type == Dead) return false;
        }
        return true;
    }


    // True si no hi ha unitat meva ni morta qualsevol
    bool sense_u_meva_ni_morta(const Pos& p) const { return sense_u_meva(p) and sense_u_morta(p); }


    // True si no hi ha cap unitat viva ni morta de cap jugador
    bool sense_unitat(const Pos& p) const { return cell(p).id == -1; }


    // True si l'enemic de la pos esta viu
    bool conte_enemic_viu(const Pos& p) const {
        int id = cell(p).id;
        if (id != -1) {  // si hi ha una tropa
            if (unit(id).player != me()) {  // si la tropa no es meva
                if (unit(id).type == Alive) return true;
            }
        }
        return false;
    }


    //True si hi ha un zombie a la pos
    bool conte_zombie(const Pos& p) const {
        // vector amb els ids del zombies
        VI z = zombies();
        for (int id : z) if (p == unit(id).pos) return true;
        return false;
    }


    // True si estic infectat
    bool infectat(int id) const { return unit(id).rounds_for_zombie != -1; }



    void mou_random_possible(const VI& vives) {
        for (int id : vives) {
            bool mogut = false;

            int i = 0;
            while (not mogut and i < 4) {
                Dir d = dirs[i];
                Pos new_pos = unit(id).pos + d;
                if (pos_ok(new_pos) and sense_waste(new_pos) and sense_u_meva_ni_morta(new_pos)) {
                    move(id, d);
                    mogut = true;
                }
                ++i;
            }
        }
    }



    // Retorna la probabilitat de guanyar atacant primer a un emimic
    double probabilitat_guanyar(const Pos& p) {

        int id_enemic = cell(p).id;
        int id_jugador = unit(id_enemic).player;

        int força_jug = strength(id_jugador);
        double força_meva = strength(me());

        if (força_meva + força_jug == 0) return 0.5;
        else return 0.3 + força_meva/(força_meva + força_jug);
    }


    // Bfs per aproparme a una unitat meva
    Dir buscar_aliat_mes_proper(const Pos& p_ini) {

        VVB visitat(60, VB(60, false));
        typedef pair<Pos, Dir> ppd;
        queue<ppd> Q;

        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {
            if (pos_ok(p_ini + d)) {
                Pos nou_pos = p_ini + d;
                visitat[nou_pos.i][nou_pos.j] = true;
                if (not sense_u_meva(nou_pos))    return d;
                else if (sense_waste(nou_pos)) Q.push(make_pair(nou_pos, d));
            }
        }
        while (not Q.empty()) {
            Pos p = Q.front().first;
            Dir d_primera = Q.front().second;
            Q.pop();

            for (Dir d : dirs) {
                if (pos_ok(p + d)) {
                    Pos nou_p = p + d;
                    if (visitat[p.i][p.j] == false) {
                        visitat[p.i][p.j] = true;
                        if (not sense_u_meva(nou_p)) return d_primera;
                        else if (sense_waste(nou_p)) Q.push(make_pair(nou_p, d_primera));
                    }
                }
            }
        }
        return Right;
    }


    // Bfs des del la meva unitat viva i restorna la direccio de si troba mes a prop el menjar o mes a prop enemic
    Dir buscar_mes_proper(const Pos& p_ini, bool modo_menjar, bool modo_enemic, bool modo_zombie) {

        VVB visitat(60, VB(60, false));

        typedef pair<Pos, Dir> ppd;

        queue<ppd> Q;  //te la posicio actual i el moviment inicial que he seguit

        //Posicio inicial mirar les 4 direccions del voltant i fer que siguin les direccions inicials a triar
        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {

            if (pos_ok(p_ini + d)) {

                Pos nou_pos = p_ini + d;

                if (cell(nou_pos).food)       return d;

                else if (sense_waste(nou_pos) and sense_u_meva(nou_pos)) {   //TODO abans sense u meva, va millor!
                    Q.push( make_pair(Pos(nou_pos), d) );
                    visitat[nou_pos.i][nou_pos.j] = true;
                }
            }
        }

        while (not Q.empty()) {

            Pos p = Q.front().first;
            Dir dir_primera = Q.front().second;
            Q.pop();

            for (Dir d : dirs) {

                if (pos_ok(p + d)) {
                    Pos nou_p = p + d;

                    if (visitat[nou_p.i][nou_p.j] == false) {

                        visitat[nou_p.i][nou_p.j] = true;


                        if (cell(nou_p).food and modo_menjar)             return dir_primera;
                        else if (conte_zombie(nou_p) and modo_zombie)     return dir_primera;
                        else if (conte_enemic_viu(nou_p) and modo_enemic) return dir_primera;

                        else if (sense_waste(nou_p) and sense_u_meva(nou_p)) { // TODO abans sense, va millor

                            Q.push( make_pair(nou_p, dir_primera) );
                        }
                    }
                }
            }
        }
        return Right;
    }


    // Buscar direccio cap a menjar segura sense enemic
    Dir escapar_anant_menjar(const Pos& p_ini) {      // no cal dir enemic ho miro al primer for

      VVB visitat(60, VB(60, false));
        typedef pair<Pos, Dir> ppd;
        queue<ppd> Q;

        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {
            if (pos_ok(p_ini + d)) {
                Pos nou_pos = p_ini + d;
                visitat[nou_pos.i][nou_pos.j] = true;
                if (cell(nou_pos).food)    return d;
                else if (sense_waste(nou_pos) and sense_unitat(nou_pos)) {
                    if (not conte_enemic_viu(nou_pos)) Q.push(make_pair(nou_pos, d));
                }
            }
        }
        while (not Q.empty()) {
            Pos p = Q.front().first;
            Dir d_primera = Q.front().second;
            Q.pop();

            for (Dir d : dirs) {
                if (pos_ok(p + d)) {
                    Pos nou_p = p + d;
                    if (visitat[p.i][p.j] == false) {
                        visitat[p.i][p.j] = true;
                        if (cell(nou_p).food)   return d_primera;
                        else if (sense_waste(nou_p)) Q.push(make_pair(nou_p, d_primera));
                    }
                }
            }
        }
        return Right;
    }



    // Quan tinc un zombie a la diagonal escaparme d'ell i anar a per menjar
    Dir escapar_un_zombie(const Pos& p_ini, const Dir& dir_zombie) {
        Pos pos_zombie = p_ini + dir_zombie;

        Pos dolenta1;
        Pos dolenta2;
        //UL
        if (dir_zombie == UL) {
            dolenta1 = Pos(pos_zombie.i - 1, pos_zombie.j);
            dolenta2 = Pos(pos_zombie.i, pos_zombie.j + 1);
        }
        //RU
        else if (dir_zombie == RU) {
            dolenta1 = Pos(pos_zombie.i, pos_zombie.j - 1);
            dolenta2 = Pos(pos_zombie.i + 1, pos_zombie.j);
        }
        //DR
        else if (dir_zombie == DR) {
            dolenta1 = Pos(pos_zombie.i - 1, pos_zombie.j);
            dolenta2 = Pos(pos_zombie.i, pos_zombie.j - 1);
        }
        //LD
        else if (dir_zombie == LD) {
            dolenta1 = Pos(pos_zombie.i - 1, pos_zombie.j);
            dolenta2 = Pos(pos_zombie.i, pos_zombie.j + 1);
        }



        // Anar a per menjar amb cami segur
        VVB visitat(60, VB(60, false));
        typedef pair<Pos, Dir> ppd;
        queue<ppd> Q;

        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {
            if (pos_ok(p_ini + d)) {
                Pos nou_pos = p_ini + d;
                visitat[nou_pos.i][nou_pos.j] = true;
                if (cell(nou_pos).food) return d; // hi ha menjar
                else if (sense_waste(nou_pos)) {
                    if (nou_pos != dolenta1 and nou_pos != dolenta2) Q.push(make_pair(nou_pos, d));
                }
            }
        }
        while (not Q.empty()) {
            Pos p = Q.front().first;
            Dir d_primera = Q.front().second;
            Q.pop();

            for (Dir d : dirs) {
                if (pos_ok(p + d)) {
                    Pos nou_p = p + d;
                    if (visitat[p.i][p.j] == false) {
                        visitat[p.i][p.j] = true;
                        if (cell(nou_p).food) return d_primera;
                        else if (sense_waste(nou_p)) {
                            Q.push(make_pair(nou_p, d_primera));
                        }
                    }
                }
            }
        }
        return UL; // esta atrapat pel zombie
    }


    // Retorna la direccio segura oposada a la diagonal del zombie
    Dir dir_sense_zombies_diagonals(const Pos& p) {

        vector<bool> esta_diagonal(4, false);
        int cont_zombies_diags = 0;
        // Trobar a quines diagonals estan els zombies
        for (Dir d : diagonals) {
            if (pos_ok(p + d)) {
                if (conte_zombie(p + d)) {
                    ++cont_zombies_diags;
                    if (d == UL)          esta_diagonal[0] = true;
                    else if (d == RU)     esta_diagonal[1] = true;
                    else if (d == DR)     esta_diagonal[2] = true;
                    else if (d == LD)     esta_diagonal[3] = true;
                }
            }
        }

        // Escollir millor direccio per escapar

        Dir dir_segura = RU; // valor fake per si no troba cap

        if (cont_zombies_diags == 1) {

            // bfs evitnat zona diagonal
            Dir d_pos_zombie = Up; //valor senyuelo
            for (int i = 0; i < 4; ++i) if (esta_diagonal[i]) d_pos_zombie = diagonals[i];
            dir_segura = escapar_un_zombie(p, d_pos_zombie);
            if (dir_segura == UL) dir_segura = RU;
        }
        else if (cont_zombies_diags == 2) {
            //Mirar si esta amunt, primers dos elements vector
            if (esta_diagonal[0] and esta_diagonal[1]) {
                if (sense_waste(p + Down)) dir_segura = Down;
            }
            else if (esta_diagonal[2] and esta_diagonal[3]) {
                if (sense_waste(p + Up)) dir_segura = Up;
            }
            else if (esta_diagonal[0] and esta_diagonal[3]) {
                if (sense_waste(p + Right)) dir_segura = Right;
            }
            else if (esta_diagonal[1] and esta_diagonal[2]) {
                if (sense_waste(p + Left)) dir_segura = Left;
            }
        }
        else if (cont_zombies_diags == 3) {
            int found = false;
            int i = 0;
            while (not found) {
                if (esta_diagonal[i] == false) {
                    dir_segura = diagonals[i];
                    found = true;
                }
                else ++i;
            }
            if (not sense_waste(p + dir_segura)) dir_segura = RU; // si te waste la pos no es segura
        }
        else if (cont_zombies_diags == 4) dir_segura = buscar_aliat_mes_proper(p); // em mossegaran, vaig cap a un aliat


        return dir_segura;
    }



    void mou_unitats_vives(const VI& vives) {

                          // Prioritat   id Dir
        priority_queue<pair<int, pair<int, Dir> > > prioritat_moviment;  // com mes prioritat mes gran l'int



        for (int id : vives) {

            Pos p = unit(id).pos;


            bool mogut = false;


            //-------------------------------------------------MODO ATAC AL VOLTANT------------------------------------------


            //1r atacar a un enemic //2n atacar a un zombie

            Dir d_zombie = DR;
            bool zombie_adjacent = false;
            int n_enemics = 0;

            priority_queue<pair<double, Dir> > enemics; // enemics adjacents de mes a menys probabilitat guanyar i dir on estan


            // buscar enemics adjacents i mirar forçes i buscar un zombie qualsevol adjacent
            for (Dir d : dirs) {
                if (pos_ok(p + d)) {

                    // Buscar enemics i mirar forçes
                    if (conte_enemic_viu(p + d)) {

                      ++n_enemics;
                                                    // pos enemic
                      double prob_win = probabilitat_guanyar(p + d);  // pot ser > de 1


                      enemics.push( make_pair(prob_win, d) );
                    }

                    // Buscar qualsevol zombi adjacent
                    if (conte_zombie(p + d)) {
                        d_zombie = d;
                        zombie_adjacent = true;
                    }
                }
            }



            // Casos enemics adjacents
            if (n_enemics >= 1) {

                double prob_win = enemics.top().first;

                if (prob_win >= 0.9) {
                    prioritat_moviment.push( make_pair(60, make_pair(id, enemics.top().second) ) );
                    mogut = true;
                }
                else if (prob_win >= 0.75) {

                    prioritat_moviment.push( make_pair(55, make_pair(id, enemics.top().second) ) );
                    mogut = true;
                }
                else if (prob_win >= 0.5) {   // Parametre a modificar, pero aixi esta be
                    prioritat_moviment.push( make_pair(50, make_pair(id, enemics.top().second) ) );
                    mogut = true;
                }

                // si es possible que perdi
                else if (prob_win < 0.5) {

                    if (zombie_adjacent) {
                        prioritat_moviment.push( make_pair(45, make_pair(id, d_zombie) ) );
                        mogut = true;
                    }
                    else {

                        if (n_enemics == 1) {
                            Dir d_escapar_enemic = escapar_anant_menjar(p);  //escapar_un_o_dos_enemics(p, enemics.top().second, UL); // a per menjar evitant enemic

                            if (sense_waste(p + d_escapar_enemic)) { // en teoria ja retorna sense
                                prioritat_moviment.push( make_pair(36, make_pair(id, d_escapar_enemic) ) );
                                mogut = true;
                            }
                        }
                        else if (n_enemics == 2) {

                            Dir d_escapar_enemic = escapar_anant_menjar(p);

                            if (sense_waste(p + d_escapar_enemic)) {
                                prioritat_moviment.push( make_pair(37, make_pair(id, d_escapar_enemic) ) );
                                mogut = true;
                            }
                        }
                        else if (n_enemics == 3) {

                            // mirar quina es la dir per escapar
                            Dir d_segura = UL;

                            bool found = false;
                            int i = 0;
                            while (not found and i < 4) {
                                if (pos_ok(p + dirs[i])) {
                                    if (not conte_enemic_viu(p + dirs[i])) {
                                        d_segura = dirs[i];
                                        found = true;
                                    }
                                }
                            }

                            if (found) {
                                if (sense_waste(p + d_segura)) {

                                    if (sense_unitat(p + d_segura)){
                                        prioritat_moviment.push( make_pair(38, make_pair(id, d_segura) ) ); // mes prio que escapar 1, aquest potser te sort i quan l'ataquen no el maten
                                        mogut = true;
                                    }
                                    else {  //tinc bloquejat l'escapada, ata
                                        prioritat_moviment.push( make_pair(39, make_pair(id, d_segura) ) );
                                        mogut = true;
                                    }

                                }
                                else {  // estic rodejat ataco a un
                                    prioritat_moviment.push( make_pair(39, make_pair(id, d_segura) ) );
                                    mogut = true;
                                }

                            }
                        }
                        else if (n_enemics == 4) {  // atacar al de mes prob
                            prioritat_moviment.push( make_pair(41, make_pair(id, enemics.top().second) ) );
                            mogut = true;
                        }
                    }
                }
            }



            // Atacar zombie qualsevol adjacent
            if (not mogut and zombie_adjacent and sense_waste(p + d_zombie)) {
                prioritat_moviment.push( make_pair(35, make_pair(id, d_zombie) ) );
                mogut = true;
            }



            // -------------------ESCAPAR ZOMBIES DIAGONALS ----------------
            if (not mogut and not infectat(id)) {
                Dir dir_segura = dir_sense_zombies_diagonals(p);

                if (dir_segura != RU) {
                    if (pos_ok(p + dir_segura) and sense_unitat(p + dir_segura) and sense_waste(p + dir_segura)) {

                      prioritat_moviment.push( make_pair(30, make_pair(id, dir_segura) ) );
                      mogut = true;
                    }
                }
            }



            // --------------------------MODO INFECTAT--------------------------
            if (not mogut) {
                if (infectat(id)) {

                    Dir d_infectat = buscar_mes_proper(p, true, true, true); // no canviar

                    if (pos_ok(p + d_infectat) and sense_unitat(p + d_infectat)) {
                        prioritat_moviment.push( make_pair(15, make_pair(id, d_infectat) ) );
                        mogut = true;
                    }
                }
            }





        //TODO si tinc mes força que la resta dels jugadors anar modo ataque no menjar


            // ----------------------------MODO MENJAR i ZOMBIES--------------------------
            if (not mogut) {

                Dir d_mes_proper = buscar_mes_proper(p, true, false, true);  // TODO prioritats respecte el que tenen mes a prop i adjacent menjar

                if (pos_ok(p + d_mes_proper) and sense_unitat(p + d_mes_proper) and sense_waste(p + d_mes_proper)) {
                    prioritat_moviment.push( make_pair(10, make_pair(id, d_mes_proper) ) );
                }
            }
        }


        // Fer tots els moviments per ordre de prioritats
        while (not prioritat_moviment.empty()) {

            int id = prioritat_moviment.top().second.first;
            Dir dir = prioritat_moviment.top().second.second;
            prioritat_moviment.pop();

            move(id, dir);
        }

    }



    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {

        int id_meu = me();

        VI vives = alive_units(id_meu);

        double st = status(id_meu);
        if (st >= 0.99) mou_random_possible(vives);

        mou_unitats_vives(vives);
    }
};



/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

