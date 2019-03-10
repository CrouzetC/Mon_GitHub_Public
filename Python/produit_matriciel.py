

#####
#
import time
#
#####


####################
#
#
#
def affiche_matrice(M):
    print()
    for ligne in M :
        print(ligne)
    print()

def resultats(A, B, C) :
    print("A :")
    affiche_matrice(A)
    print("B :")
    affiche_matrice(B)
    print("Résultat : C = A * B :")
    affiche_matrice(C)

def creation_matrices(N) :
    A = []
    B = []
    for i in range(N) :
        l1 = [i+j for j in range(N)]
        A.append(l1)
        l2 = [i+j for j in range(N)]
        B.append(l2)
    return [A, B]

def produit_matriciel_time(A, B) :
    ''' Renvoie une liste [C, dt] où :
    - C est le résultat du produit matriciel
    - dt est la durée (en s) prise pour effectuer le produit
    (hors création de la matrice C) '''

    N = len(A)
    C = [ N*[0] for i in range(N) ]

    t1 = time.time()
    for i in range(N) :
        for j in range(N) :
            s=0
            for k in range(N) :
                s += A[i][k]*B[k][j]
            C[i][j] = s

    t2 = time.time()
    return [C, t2-t1]

def start_simple() :
    ''' Fait un produit entre deux matrices '''

    print()
    N = int( input("Taille des matrices : ") )
    print()
    AB = creation_matrices(N)
    A = AB[0]
    B = AB[1]
    C = produit_matriciel_time(A, B)[0]
    resultats(A, B, C)

def start_time() :
    ''' Donne des durées du produits matriciels pour différentes tailles de matrices '''

    tailles = [10, 50, 100, 150, 200, 250]
    durees_ms = []
    for taille in tailles :
        AB = creation_matrices(taille)
        A = AB[0]
        B = AB[1]
        duree = produit_matriciel_time(A, B)[1]
        durees_ms.append(1000*duree)

    for i in range(len(tailles)) :
        print("Taille : " + str(tailles[i]) + " ; durée : " + str(durees_ms[i]) + " ms ; racine cubique de la durée en ms : " + str(durees_ms[i]**(1/3)) + ".")
    print()
#
#
#
####################


start_time() # Fait un produit entre 2 matrices





