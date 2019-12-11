

 ### Paramètres :

# nombre de points
N = 30

eps=0.00000001

 ###



 ###

from random import random

from pylab import *
from math import *
from copy import *

 ###


##### Fonction(s) :

def tri_selon_theta(L) :
    L_theta=[]
    xG, yG = 0, 0
    for [x,y] in points :
        xG+=x
        yG+=y
    xG=xG/N
    yG=yG/N
    for [x,y] in points :
        v=[x-xG,y-yG]
        norme=sqrt(v[0]**2+v[1]**2)
        if y<yG :
            L_theta.append( acos( v[0]/norme ) )
        elif abs(v[0]-norme) < eps :
            L_theta.append(0)
        elif abs(v[1]+norme) < eps :
            L_theta.append(pi)
        else :
            L_theta.append( 2*pi - acos( v[0]/norme ) )

    ### Algorithme Tri-bulle :
    for i in range(N-1) :
        for j in range (0,N-1-i) :
            if L_theta[j]>L_theta[j+1] :
                cte=[ copy(L[j+1]), L_theta[j+1] ]
                L_theta[j+1]=L_theta[j]
                L_theta[j]=cte[1]
                L[j+1]=copy(L[j])
                L[j]=copy(cte[0])
    return 0
        

def scalaire(u,v) :
    return u[0]*v[0]+u[1]*v[1]


def triangulation(polygone) : # !! Complexité en O(n^3) dans le pirecas (et O(n^2) dans le meilleur cas)

    #################### L'étape qui suit n'est en fait pas utilisée par la suite...
    ## On veut : normal[i] = True ssi le vecteur perpendiculaire à l'arête entre les points i et i+1 (et orienté vers l'extérieur de polygone) pointe vers la moitié droite du plan (ou exactement vers le haut, mais pas exactement vers le bas).
    #normal=[]
    #for i in range(N-1) :
    #    if (polygone[i+1][0]-polygone[i][0] > 0) | ( (polygone[i][0] == polygone[i+1][0]) & (polygone[i+1][1]-polygone[i][1] >= 0) ) :
    #        normal=normal+[True]
    #    else :
    #        normal=normal+[False]
    #normal = normal + [ ( (polygone[0][0]-polygone[N-1][0] > 0) | ( (polygone[N-1][0] == polygone[0][0]) & (polygone[0][1]-polygone[N-1][1] >= 0) ) ) ]
    #
    ## Puis on inverse toutes les normales si on s'était trompé d'orientation :
    #i_min = 0 # i_min correspond à l'indice du point d'abscisse la plus faible
    #for i in range(1,N) :
    #    if polygone[i][0] < polygone[i_min][0]:
    #        i_min=i
    #v1=[ polygone[(i_min-1+N)%N][0] - polygone[i_min][0] , polygone[(i_min-1+N)%N][1] - polygone[i_min][1] ]
    #v2=[ polygone[(i_min+1)%N][0] - polygone[i_min][0] , polygone[(i_min+1)%N][1] - polygone[i_min][1] ]
    #if v1[0]*v2[1] - v1[1]*v2[0] < 0 :
    #    for i in range(N) :
    #        normal = False
    #################### Fin de l'étape des normales.

    N=len(polygone)
    # Ici, on se contente en fait d'une seule variable booléenne 'normal' pour connaître la direction de l'intérieur du polygone (cf la photo "Explications 'normal' et 'triangle_valide (1)'" dans le dossier "Explications")
    i_min = 0 # i_min correspond à l'indice du point d'abscisse la plus faible
    # (cette étape de recherche de i_min n'est sans doutes PAS nécessaire)
    for i in range(1,N) :
        if polygone[i][0] < polygone[i_min][0]:
            i_min=i
    
    v1=[ polygone[(i_min-1+N)%N][0] - polygone[i_min][0] , polygone[(i_min-1+N)%N][1] - polygone[i_min][1] ]
    v2=[ polygone[(i_min+1)%N][0] - polygone[i_min][0] , polygone[(i_min+1)%N][1] - polygone[i_min][1] ]

    normal = (v1[0]*v2[1] - v1[1]*v2[0] > 0) # On se sert du calcul du déterminant d'une matrice ( matrice 2x2 : [u,v] ) pour connaître le signe de l'angle saillant formé par u et v
    
    triangulation = []
    liste_triangles = [] # liste_triangle contiendra des triplets : chaque triplet contient les indices des 3 points formés par un triangle
    liste_indices = [ i for i in range(N)]
    poly = deepcopy(polygone)
    while (len(triangulation) < N-3) :   # pour un polygone de N points, il faut tracer N-3 traits pour le triangulariser
        N_poly=len(poly)
        i=-1 # 'i' est l'indice tel que lui et les trois points suivants constituent un triangle à extraire du polygone 'poly'
        triangle_valide=False
        while (triangle_valide==False) :
            i+=1
            a=poly[i]
            b=poly[(i+1)%N_poly]
            c=poly[(i+2)%N_poly]
            # Un triangle est valide ssi :
            #(1) l'angle saillant entre les vecteurs poly[i]-poly[i+1] et poly[i+2]-poly[i+1] 'passe' par l'intérieur du polygone
            n = [ b[1]-a[1], -(b[0]-a[0]) ]
            if normal==False :
                n=[-n[0],-n[1]]
            triangle_valide = ( scalaire( n , [ c[0]-b[0], c[1]-b[1] ] ) > 0 )
            #(2) aucun autre point du polygone n'appartient au triangle (côtés du triangle exclus)
            j=i+3
            while (j < N_poly+i) & (triangle_valide) :
                if appartient_a_triangle( a, b, c, poly[j%N_poly] ) :
                    triangle_valide=False
                j+=1
        triangulation = triangulation + [ [ poly[i], poly[(i+2)%N_poly] ] ]
        liste_triangles = liste_triangles + [ [ liste_indices[i] , liste_indices[(i+1)%N_poly] , liste_indices[(i+2)%N_poly] ] ]
        poly = poly[0:i+1]+poly[i+2:N_poly]
        liste_indices = liste_indices[0:i+1] + liste_indices[i+2:N_poly]
    liste_triangles = liste_triangles + [ [ liste_indices[0] , liste_indices[1] , liste_indices[2] ] ]

    return_value = [ triangulation , liste_triangles ]
    return return_value

def points_tous_alignes(L) :
    if len(L) <= 2 :
        return True
    
    alignes=True
    i=2
    while (alignes) & (i<len(L)) :
        delstaX=111
        deltaY=1
        if abs(1) - abs(1) > eps :
            alignes=False
        i+=1
    return alignes


def appartient_a_triangle( A, B, C, P ): # i, j et k sont les indices des points du triangle
    
    def f(a,b,c,v) :
        
        gamma = scalaire( [ b[0]-a[0] , b[1]-a[1] ] , [ c[0]-a[0] , c[1]-a[1] ] )
        mu = gamma / ( (c[0]-a[0])**2+(c[1]-a[1])**2)
        n = [   b[0] + (mu-1)*a[0] - mu*c[0]   ,   b[1] + (mu-1)*a[1] - mu*c[1]   ]
        return (scalaire( [ v[0]-a[0], v[1]-a[1] ] , n ) >= 0)
    
    
    appartient = f(A,B,C,P) & f(B,C,A,P) & f(C,A,B,P)
    #if appartient :
    #    plot([A[0],B[0],C[0]],[A[1],B[1],C[1]])
    #    scatter([P[0]],[P[1]])
    #    show()
    return appartient

#####


 ### Création des points :

points=[]
for i in range(N) :
    x=random()
    y=random()
    points.append([x,y])

 ###



 ### Création des arêtes :

# liste des indices correspondant aux points,
# dans l'ordre dans lequel ils sont reliés.
aretes=[]

# On trie les points par ordre croissant de "théta_" (explications sur le doc .png du même dossier)
aretes = deepcopy(points)
tri_selon_theta(aretes)
aretes.append(copy(aretes[0]))

 ###



 ### Affichage :

X1=[ points[i][0] for i in range(N) ]
Y1=[ points[i][1] for i in range(N) ]

scatter(X1,Y1)

X=[ aretes[i][0] for i in range(N+1) ]
Y=[ aretes[i][1] for i in range(N+1) ]

plot(X,Y)

show()

scatter(X1,Y1)
plot(X,Y)

for couple_vect in triangulation(aretes[0:len(aretes)-1])[0] :
    X=[ couple_vect[0][0], couple_vect[1][0] ]
    Y=[ couple_vect[0][1], couple_vect[1][1] ]
    plot( X , Y )


show()

 ###

 ### Autres exemples :

aretes2 = [ [10,i] for i in range(10) ] + [ [9, 9-i] for i in range (9) ] + [ [3,i+1] for i in range(9) ] + [ [-1,9-i] for i in range(10) ] + [ [10,0] ]

N=len(aretes2)-1

X=[ aretes2[i][0] for i in range(N+1) ]
Y=[ aretes2[i][1] for i in range(N+1) ]

plot(X,Y)

for couple_vect in triangulation(aretes2[0:len(aretes2)-1])[0] :
    X=[ couple_vect[0][0], couple_vect[1][0] ]
    Y=[ couple_vect[0][1], couple_vect[1][1] ]
    plot( X , Y )

show()


aretes3 = [ [0,0] ] + [ [i,10] for i in range(11) ] + [ [10-i,9] for i in range(10) ] + [ [i,1] for i in range(1,11) ] + [ [10-i,0] for i in range(11) ]
N=len(aretes3)-1
X=[ aretes3[i][0] for i in range(N+1) ]
Y=[ aretes3[i][1] for i in range(N+1) ]
plot(X,Y)
for couple_vect in triangulation(aretes3[0:len(aretes3)-1])[0] :
    X=[ couple_vect[0][0], couple_vect[1][0] ]
    Y=[ couple_vect[0][1], couple_vect[1][1] ]
    plot( X , Y )
show()

for triplet_indices in triangulation(aretes3[0:len(aretes3)-1])[1] :
    X=[ aretes3[ triplet_indices[i] ][0] for i in range(3) ] + [ aretes3[triplet_indices[0]][0] ]
    Y=[ aretes3[ triplet_indices[i] ][1] for i in range(3) ] + [ aretes3[triplet_indices[0]][1] ]
    plot(X,Y)
show()


x0=2
y0=2
r=1.4
R=2
aretes4 = [ [ x0+r*cos( pi/4+i/100*3*pi/2 ), y0+r*sin( pi/4+i/100*3*pi/2 )] for i in range(101)] + [ [ x0+R*cos( pi/4+i/100*3*pi/2 ) , y0 + R*sin( pi/4 + (i/100)*3*pi/2 )] for i in range(100,-1,-1)] + [ [x0+r*cos(pi/4), y0+r*sin(pi/4)] ]
N=len(aretes4)-1
X=[ aretes4[i][0] for i in range(N+1) ]
Y=[ aretes4[i][1] for i in range(N+1) ]
plot(X,Y)
for couple_vect in triangulation(aretes4[0:len(aretes4)-1])[0] :
    X=[ couple_vect[0][0], couple_vect[1][0] ]
    Y=[ couple_vect[0][1], couple_vect[1][1] ]
    plot( X , Y )
show()

 ###
