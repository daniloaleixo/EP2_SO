import sys
import os

NUMERO_ENTRADAS = 2
TAMANHO_CICLISTAS = { 'peq': '_5_', 'med': '_30_', 'gra':'_60_' }
TAMANHO_PISTA = { 'peq': '_250_', 'med': '_1000_', 'gra':'_5000_' }
NOME_ARQS_RESULTADOS = ['p_peq_c_peq', 'p_peq_c_med', 'p_peq_c_gra',
						'p_med_c_peq', 'p_med_c_med', 'p_med_c_gra',
						'p_gra_c_peq', 'p_gra_c_med', 'p_gra_c_gra']



tempos_CPU = [[],[],[],[],[],[],[],[],[]]
page_faults = [[],[],[],[],[],[],[],[],[]]
voluntary_context_switches = [[],[],[],[],[],[],[],[],[]]
context_switches = [[],[],[],[],[],[],[],[],[]]


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
	print "get_info_from_tests <diretorio onde estao as saidas> <diretorio onde ficarao os resultados>"
	print "\n"
	exit(0)

diretorio_saidas = sys.argv[1]
diretorio_resultados = sys.argv[2];
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

	# print content[12].split(' ')[-1]
	# print content[14].split(' ')[-1]
	#print content[4].split(' ')[-1]

	tempos_CPU[indice].append(content[4].split(' ')[-1])
	page_faults[indice].append(content[12].split(' ')[-1])
	voluntary_context_switches[indice].append(content[13].split(' ')[-1])
	context_switches[indice].append(content[14].split(' ')[-1])
	#print tempo

#print tempos_CPU

for i in range(0, len(NOME_ARQS_RESULTADOS)):
	file_tempos = open(diretorio_resultados + NOME_ARQS_RESULTADOS[i] + ".txt", "w")
	file_tempos.write("Uso de CPU: \n")
	for j in range(0, len(tempos_CPU[i])):
		file_tempos.write(tempos_CPU[i][j])

	file_tempos.write("\nPage Faults: \n")
	for j in range(0, len(page_faults[i])):
		file_tempos.write(page_faults[i][j])
	
	file_tempos.write("\n Voluntary Context Switches: \n")
	for j in range(0, len(voluntary_context_switches[i])):
		file_tempos.write(voluntary_context_switches[i][j])


	file_tempos.write("\n Involuntary Context Switches: \n")
	for j in range(0, len(context_switches[i])):
		file_tempos.write(context_switches[i][j])

	file_tempos.close()



