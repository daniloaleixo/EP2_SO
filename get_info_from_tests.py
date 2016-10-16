import sys
import os

NUMERO_ENTRADAS = 1
TAMANHO_CICLISTAS = { 'peq': '_5_', 'med': '_30_', 'gra':'_60_' }
TAMANHO_PISTA = { 'peq': '_250_', 'med': '_1000_', 'gra':'_5000_' }



tempos = [[],[],[],[],[],[],[],[],[]]
# print tempos

# existem 9 combinacoes para pistas e ciclistas elas sao 
 # 0 - pista peq. e ciclista peq.
 # 1 - pista peq. e ciclista med.
 # 2 - pista peq. e ciclista gra.
 # 3 - pista med. e ciclista peq.
 # 4 - pista med. e ciclista med.
 # 5 - pista med. e ciclista gra.
 # 6 - pista gra. e ciclista peq.
 # 7 - pista gra. e ciclista med.
 # 8 - pista gra. e ciclista gra.



if len(sys.argv) < NUMERO_ENTRADAS + 1:
	print "Modo de uso\n\n"
	print "get_info_from_tests <diretorio onde estao as saidas> "
	print "\n"
	exit(0)

diretorio_saidas = sys.argv[1]
#print diretorio_saidas

arquivos_saida = os.listdir(diretorio_saidas)
#print arquivos_saida;

for i in range(0, len(arquivos_saida)):
	if TAMANHO_PISTA['peq'] in arquivos_saida[i] and TAMANHO_CICLISTAS['peq'] in arquivos_saida[i]:
		indice = 0
	elif TAMANHO_PISTA['peq'] in arquivos_saida[i] and TAMANHO_CICLISTAS['med'] in arquivos_saida[i]:
		indice = 1
	elif TAMANHO_PISTA['peq'] in arquivos_saida[i] and TAMANHO_CICLISTAS['gra'] in arquivos_saida[i]:
		indice = 2
	elif TAMANHO_PISTA['med'] in arquivos_saida[i] and TAMANHO_CICLISTAS['peq'] in arquivos_saida[i]:
		indice = 3
	elif TAMANHO_PISTA['med'] in arquivos_saida[i] and TAMANHO_CICLISTAS['med'] in arquivos_saida[i]:
		indice = 4
	elif TAMANHO_PISTA['med'] in arquivos_saida[i] and TAMANHO_CICLISTAS['gra'] in arquivos_saida[i]:
		indice = 5
	elif TAMANHO_PISTA['gra'] in arquivos_saida[i] and TAMANHO_CICLISTAS['peq'] in arquivos_saida[i]:
		indice = 6
	elif TAMANHO_PISTA['gra'] in arquivos_saida[i] and TAMANHO_CICLISTAS['med'] in arquivos_saida[i]:
		indice = 7
	elif TAMANHO_PISTA['gra'] in arquivos_saida[i] and TAMANHO_CICLISTAS['gra'] in arquivos_saida[i]:
		indice = 8
	else: 
		print 'nao manjo'

	with open('saidas/' + arquivos_saida[i]) as f:
	    content = f.readlines()
	#print content[4]
	#print content[4].split(' ')[-1]

	tempos[indice].append(content[4].split(' ')[-1].replace("\n", ""))
	#print tempo

print tempos


