###Programa para experimentar con archivos txt y tkinter

###Creacion de archivo y verificacion de existencia
try:
    t1=open("test_file.txt", "x")
except:
    print("ya existe")
'''
Escritura 
t1=open("test_file.txt", "w")
titulo="Practica 1"
texto="Vamos experimentando porque pues aja\nExperimento el texto\n y yap"
saltos="\n\n"
t1.write(titulo)
t1.write(saltos)
t1.writelines(texto)
t1.close()'''

t1=open("test_file.txt", "r")
###t1.readline o readlines