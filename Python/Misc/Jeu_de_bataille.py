
import random as rnd

###

def melange(L) :
    N = len(L)
    for i in range(N-1) :
        echange = rnd.randint(i,N-1)
        tmp = L[i]
        L[i] = L[echange]
        L[echange] = tmp

###

###

deck1 = []

for couleur in [" <3", " <>", " -8o", " -8>"] :
    deck1.append("AS"+couleur)
    deck1.append("Roi"+couleur)
    deck1.append("Dame"+couleur)
    deck1.append("Valet"+couleur)
    for num in range(2,11,1) :
        deck1.append(str(num)+couleur)

melange(deck1)
deck2 = deck1[26:]
deck1 = deck1[:26]

###

print("Appuyez sur Entrée pour jouer une carte")
print("(saisir exit pour abandonner)")
print()

pile1 = []
pile2 = []
fin = False
while fin == False:
    if input()=="exit" :
        fin = True
        print("vous avez abandonné (appuyer sur Entrée pour quitter)")
        input()
    else :
        print("Vous jouez :")
        print("    "+deck1[0])
        print("L'adversaire joue :")
        print("    "+deck2[0])
        print()

        # On vérifie qui a gagné la manche
        cartes = [deck1[0][0], deck2[0][0]]
        for i in [0,1] :
            if cartes[i]=="V":  cartes[i] = 11
            elif cartes[i]=="D":  cartes[i] = 12
            elif cartes[i]=="R":  cartes[i] = 13
            elif cartes[i]=="A":  cartes[i] = 14
            else : cartes[i] = int(cartes[i])
            
        if cartes[0] > cartes[1] :
            victoire = 1 # ce n'est pas un compteur de victoires, juste une variable temporaire
            defaite = 0
        elif cartes[0] < cartes[1] :
            victoire = 0
            defaite = 1
        else :
            victoire = 0
            defaite = 0
        
        # on modifie les decks et piles(=défausses) en conséquences
        pile1 = pile1 + deck1[0:1-defaite] + deck2[0:victoire]  # c'est un peu tordu ici, mais ça marche
        pile2 = pile2 + deck2[0:1-victoire] + deck1[0:defaite]
        deck1 = deck1[1:] # fonctionne même s'il ne reste qu'un seul élément
        deck2 = deck2[1:]
        
        # on traite le cas où un(des) deck(s) est(sont) vide(s) :
        if len(deck1) == 0 :
            if len(pile1) == 0 :
                fin = True
                print("Vous avez perdu ! (saisir Entrée pour quitter)")
                input()
            deck1 = pile1[:]
            melange(deck1)
            pile1 = []
            print("Vous mélangez votre défausse et la récupérez !")
            input()
        if len(deck2) == 0 :
            if len(pile2) == 0 :
                fin = True
                print("Vous avez gagné ! (saisir Entrée pour quitter)")
                input()
            deck2 = pile2[:]
            melange(deck2)
            pile2 = []
            print("Votre adversaire mélange sa défausse et la récupère !")
            input()
        print("Vous : "+str(len(deck1)+len(pile1))+" cartes ; adversaire : "+str(len(deck2)+len(pile2))+" cartes.")
        print()

