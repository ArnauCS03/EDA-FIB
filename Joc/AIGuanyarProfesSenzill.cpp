#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME GuanyarProfesSenzill        // Vaig a per menjar, i ataco si tinc adjacent enemic, despres zombie, despres menjar, si infectat, a pel mes proper


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
    const vector<Dir> dirs = {Left, Right, Down, Up};     //Diferent


    // TYPEDEFS -------------------------------------------------------------------------------
    typedef vector<int> VI;
    typedef vector<VI> VVI;
    typedef vector<bool> VB;
    typedef vector<VB> VVB;
    typedef vector<Dir> VD;
    typedef vector<VD> VVD;


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
        for (int id : z) {                //miro totes les pos dels zombies i a veure si coincideix amb p
            if (p == unit(id).pos) return true;
        }
        return false;
    }


    // True si estic infectat
    bool infectat(int id) const { return unit(id).rounds_for_zombie != -1; }


    // Retorna el jugador que controla el enemic de la pos p
    int jugador_enemic(const Pos& p) const {
        int id = cell(p).id;
        return unit(id).player;
    }


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



    // Bfs des del la meva unitat viva fins al menjar
    Dir buscar_menjar(const Pos& p_ini) {

        VVB visitat(60, VB(60, false));

        typedef pair<Pos, Dir> ppd;

        queue<ppd> Q;  //te la posicio actual i el moviment inicial que he seguit

        //Posicio inicial mirar les 4 direccions del voltant i fer que siguin les direccions inicials a triar
        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {

            if (pos_ok(p_ini + d)) {

                Pos nou_pos = p_ini + d;

                if (cell(nou_pos).food) return d;
                else if (sense_waste(nou_pos) and sense_u_meva(nou_pos) and sense_u_morta(nou_pos)) {
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

                        if (cell(nou_p).food) return dir_primera;
                        else if (sense_waste(nou_p)) {

                            Q.push( make_pair(nou_p, dir_primera) );
                        }
                    }
                }
            }
        }
        return Right;
    }




    // Bfs des del la meva unitat viva i restorna la direccio de si troba mes a prop el menjar o mes a prop enemic
    Dir buscar_mes_proper(const Pos& p_ini) {

        VVB visitat(60, VB(60, false));

        typedef pair<Pos, Dir> ppd;

        queue<ppd> Q;  //te la posicio actual i el moviment inicial que he seguit

        //Posicio inicial mirar les 4 direccions del voltant i fer que siguin les direccions inicials a triar
        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {

            if (pos_ok(p_ini + d)) {

                Pos nou_pos = p_ini + d;

                if (cell(nou_pos).food) {       // al costat tinc FOOD
                    if (round() < 10)  cerr << "El menjar esta al meu costat, vaig: " << d << endl;
                    return d;
                }

                else if (conte_zombie(nou_pos)) return d;
                else if (sense_waste(nou_pos) ) {

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

                        if (cell(nou_p).food) {               // Trobo menjar


                            return dir_primera;
                        }
                        else if (conte_enemic_viu(nou_p)) {   // Trobo enemic



                            return dir_primera;
                        }
                        else if (conte_zombie(nou_p)) {
                            return dir_primera;
                        }
                        else if (sense_waste(nou_p)) { // and sense_u_morta(nou_p)) { //and sense_u_meva(nou_p)) { //TODO probar amb

                            Q.push( make_pair(nou_p, dir_primera) );
                        }
                    }
                }
            }
        }
        return Right;
    }




    // Bfs
    Dir buscar_enemic(const Pos& p_ini) {

        VVB visitat(60, VB(60, false));

        typedef pair<Pos, Dir> ppd;

        queue<ppd> Q;  //te la posicio actual i el moviment inicial que he seguit

        //Posicio inicial mirar les 4 direccions del voltant i fer que siguin les direccions inicials a triar
        visitat[p_ini.i][p_ini.j] = true;
        for (Dir d : dirs) {

            if (pos_ok(p_ini + d)) {

                Pos nou_pos = p_ini + d;

                if (conte_enemic_viu(nou_pos)) {

                    return d;
                }


                else if (sense_waste(nou_pos) ) {//and sense_u_meva(nou_pos)) { //and sense_u_morta(nou_pos)) {  //TODO canviar aixo
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


                        if (conte_enemic_viu(nou_p)) {   // Trobo enemic

                            return dir_primera;
                        }

                        else if (sense_waste(nou_p)) { // and sense_u_morta(nou_p)) { //and sense_u_meva(nou_p)) { //TODO probar amb

                            Q.push( make_pair(nou_p, dir_primera) );
                        }
                    }
                }
            }
        }
        return Right;
    }



    void mou_unitats_vives(const VI& vives) {

      //TODO canvi ordre vec dirs
        for (int id : vives) {

            Pos p = unit(id).pos;


              bool mogut = false;


            //-------------------------------------------------MOTDO ATAC AL VOLTANT------------------------------------------


            //1r atacar a un enemic //2n atacar a un zombie
            Dir d_enemic = DR;
            Dir d_zombie = DR;

            // buscar que tinc adjacent
            for (Dir d : dirs) {
                if (pos_ok(p + d)) {
                    if (conte_enemic_viu(p + d)) d_enemic = d;
                    if (conte_zombie(p + d)) d_zombie = d;
                }
            }

            if (d_enemic != DR) {  // si tinc enemic al costat, l'ataco
                move(id, d_enemic);
                mogut = true;
            }
            else if (d_zombie != DR) { //si tinc un zombie al costat, l'ataco
                move(id, d_zombie);
                mogut = true;
            }


            // MODO INFECTAT
            if (not mogut) {
                if (infectat(id)) {

                    Dir d_infectat = buscar_mes_proper(p);

                    if (pos_ok(p + d_infectat) and sense_unitat(p + d_infectat)) {
                        move(id, d_infectat);
                        mogut = true;
                    }

                }
            }


            // ----------------------------MODO a pel mejar--------------------------
            if (not mogut) {

                Dir d_mes_proper = buscar_menjar(p);

                if (pos_ok(p + d_mes_proper) and sense_unitat(p + d_mes_proper)) {
                    move(id, d_mes_proper);
                }
            }


            }
        }



    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {

        int id_meu = me();

        VI vives = alive_units(id_meu);

        // Si a punt esgotar temps CPU, moure pos ok random
        double st = status(id_meu);
        if (st >= 0.99) mou_random_possible(vives);

        mou_unitats_vives(vives);
    }
};



/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

