from math import sqrt
from math import ceil
from math import log
from math import min
from math import max
import matplotlib.pyplot as plt
from bitstring import BitArray
from bitstring import Bits

# CALCULATIONS
alpha_worst_by_x = lambda x: max(0,min(1, sqrt(x*(x-1))-1 ))
alpha_worst_by_b = lambda b: max(0,min(1, (2**(b-2))-(2**(-b-2))-1 ))
alpha_best_by_x = lambda x: 1 if x is 1 else 0
alpha_best_by_b = lambda b: 0
#	Returns precision achieved at value 'x'
#	with value-history parameter 'alpha'
f = lambda x, alpha: (1.0+alpha)/((x+alpha)*(x+alpha+1.0))
#	Returns precision achieved at value 'x'
#	assuming a worst-performance value-history parameter
f_worst = lambda x: f(x, alpha_worst_by_x(x))
#	Returns precision achieved at value 'x'
#	assuming a best-performance value-history parameter
f_best = lambda x: f(x, alpha_best_by_x(x))

#	Returns minimum value that can be stored in 'b' bits
#	in order to have at most a precision density 'bar',
#	with value-history parameter 'alpha'
f_x_min = lambda b, bar, alpha: \
		-alpha-0.5 + sqrt(((alpha+1)*(bar**-b))+0.25)
#	Returns minimum value that can be stored in 'b' bits
#	in order to have at most a precision density of 'bar', 
#	assuming a worst-performance value-history parameter
f_x_min_worst = lambda b, bar: \
		f_x_min(b,bar, alpha_worst_by_b(b))

#	Returns maximum number of bits that can be used to store 'x'
#	in order to have at most a precision density of 'bar', 
#	with value-history parameter 'alpha'
f_b_max = lambda x, bar, alpha: \
	log(alpha+1,bar) - log(x+alpha,bar) - log(x+alpha+1,bar)
#	Returns maximum number of bits that can be used to store 'x'
#	in order to have at most a precision density of 'bar', 
#	assuming a worst-performance value-history parameter
f_b_max_worst = lambda x, bar: f_b_max(x,bar, alpha_worst_by_x(x))



# CONSTANTS
NO_OF_BITS = 64
BAR = 10**(-0.25)

# FUNCTIONS

# Plots the precision increase achieved (y coord.) by adding a term a_n (x coord.)
# for both best_case and worst_case value-history parameters
def plot_minmax_zoom_vs_value():
	# Init Figure
	#fig = plt.figure()
	#ax = fig.add_subplot(111)
	labels = (
	    "CF precision, alpha=0",
	    "CF precision, alpha=1",
	    "floating-point precision",
	    "CF precision min.",
	    "CF precision max."
	)
	lines=[]
	
	# Constants
	samples_perbit=10
	samples=1+samples_perbit*(NO_OF_BITS-1)
	alpha_samples=10
	x_bits = [1+float(i)/samples_perbit for i in range(samples)] #bits used to represent x
	x = [2**(i-1) for i in x_bits]
	alphas = [float(i)/alpha_samples for i in range(alpha_samples+1)]
	f_precision_CT_temp = lambda a: [f(x[i],a)**(1.0/x_bits[i]) for i in range(samples)]
	# Plot Alpha Curves
	lines += plt.plot(
		x_bits, 
		f_precision_CT_temp(0), 
		color=(0.3,0.5,1), 
		label = labels[0]
	)
	for a in alphas[1:-1]:
		plt.plot(x_bits, f_precision_CT_temp(a), color=(0.3,0.5*(a+1),1-a))
	lines += plt.plot(
		x_bits, 
		f_precision_CT_temp(1), 
		color=(0.3,1,0),
		label = labels[1]
	)
	
	# Constants
	x_bits = range(1,NO_OF_BITS+1) #bits used to represent x
	x = [2**(i-1) for i in x_bits]
	# * this is a some-what conservative estimate (i.e. in reality, a larger x 
	#		could be represented in said number of bits
	# ** this does not take into account the bit-loss from storing mixed-width integers
	
	# precision factor of binary floating-point numbers (i.e., a constant 1/2)
	prec_binary = [0.5 for i in x]
	# Lower-bound for precision-factor of Continued Fractions
	prec_CF_low = [f_best(x[i])**(1.0/x_bits[i]) for i in range(NO_OF_BITS)]
	# Upper-bound for precision factor of Continued Fractions
	prec_CF_high = [f_worst(x[i])**(1.0/x_bits[i]) for i in range(NO_OF_BITS)]
	
	# Plot 
	w = range(1,NO_OF_BITS+1)
	lines += plt.plot(x_bits, prec_binary,  'y-', label=labels[2])
	lines += plt.plot(x_bits, prec_CF_low,  'm^', label=labels[3])
	lines += plt.plot(x_bits, prec_CF_high, 'mv', label=labels[4])
	plt.legend(lines, labels)
	plt.show()
# End function


def plot_zoom_per_bit_vs_value():
	# Constants
	x = [2**i for i in range(NO_OF_BITS)]
	# * this is a some-what conservative estimate (i.e. in reality, a larger x 
	#		could be represented in said number of bits
	# ** this does not take into account the bit-loss from storing mixed-width integers
		
	# precision factor of binary floating-point numbers (i.e., a constant 1/2)
	prec_binary = 0.5
	
	# precision factor of continuing block encoding (w/ block size of 1 bit)
	DATA_BITS=1
	prec_cont_block_1_low_logx = []
	prec_cont_block_1_low = []
	prec_cont_block_1_high_logx = [0]
	prec_cont_block_1_high = [f_worst(1)**(0.5)]
	for i in range(1,NO_OF_BITS):
		prec_cont_block_1_high_logx.append(log((2**i)-1,2)) 
		prec_cont_block_1_high.append(
			f_worst((2**i)-1)**(1.0/((1+(i-1)/DATA_BITS)*(DATA_BITS+1)))
		)
		prec_cont_block_1_high_logx.append(i)
		prec_cont_block_1_high.append(
			f_worst(2**i)**(1.0/((1+i/DATA_BITS)*(DATA_BITS+1)))
		)
	# ^TODO 
	# lowest upper limit for optimal coding
	bar = lowest_bar_inwhich_code_exists()
	
	# precision factor for coded values
	prec_optimal_low = []
	prec_optimal_low_logx = []
	prec_optimal_high = []
	prec_optimal_high_logx = []
	
	#ics = itemcounts_by_bitcount_from_bar(BAR, int(2*f_b_max_worst(2**NO_OF_BITS, BAR)))
	#optimize_itemcounts_per_bitcount(ics)
	#sum=0
	#for b in range(len(ics)):
	#	if not ics[b]==0:
	#		sum+=1
	#		prec_optimal_high.append(f_worst(sum)**(1.0/b))
	#		prec_optimal_high_logx.append(log(sum,2))
	#		sum+=ics[b]-1
	#		prec_optimal_high.append(f_worst(sum)**(1.0/b))
	#		prec_optimal_high_logx.append(log(sum,2))
	#del sum
	#del ics
	sum=0
	b=0
	while sum <= 2**NO_OF_BITS:
		if not itemcount_by_bitcount_scheme2(b)==0:
			sum+=1
			prec_optimal_high.append(f_worst(sum)**(1.0/b))
			prec_optimal_high_logx.append(log(sum,2))
			sum+=itemcount_by_bitcount_scheme2(b)-1
			prec_optimal_high.append(f_worst(sum)**(1.0/b))
			prec_optimal_high_logx.append(log(sum,2))
		b+=1
	
	
	lexibinary_values = [2**((i+1)/2)-i%2 for i in range(NO_OF_BITS*2)]
	lexibinary_logx = [log(x,2) for x in lexibinary_values]
	#lexibinary_bits = [i+1-i%2 for i in range(NO_OF_BITS)]
	lexibinary_prec = [f_worst(lexibinary_values[i])**(1.0/(i+1-i%2))
		for i in range(NO_OF_BITS*2)]
	
	# Plot
	labels = (
	    "floating-point binary precision",
	    #,
	    "VLQ precision (data bits/block = "+str(DATA_BITS)+")",
	    "CF precision worst-case limit",
	    "lexibinary precision",
	    #,
   	    "CF precision"
	)
	lines = []
	lines += plt.plot((0, NO_OF_BITS+1), (prec_binary,prec_binary),  'y-')
	#lines += plt.plot(prec_cont_block_1_low_logx, prec_cont_block_1_low, '0.75')
	lines += plt.plot(prec_cont_block_1_high_logx, prec_cont_block_1_high, '0.75')
	lines += plt.plot((0, NO_OF_BITS+1), (BAR,BAR),  'm--')
	lines += plt.plot(lexibinary_logx, lexibinary_prec, "b-")
	#lines += plt.plot(prec_optimal_low_logx, prec_optimal_low, 'r-')
	lines += plt.plot(prec_optimal_high_logx, prec_optimal_high, 'r-')
	plt.legend(lines, labels)
	plt.show()
# End function


def plot2():
	x_max = [int(ceil(f_x_min_worst(i,0.5))) for i in range(1,NO_OF_BITS+2)]
	log_x_max = [log(x,2) for x in x_max]
	#TODO plot lines from x_max[i],i to x_max[i+1]-1,i
	
	plt.plot(range(1,NO_OF_BITS+2), log_x_max, 'g')
	plt.show()

# Arrange items by their limit of bits -> input[bitlimit] == [items]
# returns whether a binary code is successfully generated
def code_for_itemlists_per_bit(itemlists_by_bitcount, dict_codes):
	dict_codes.clear()
	_prefixes = [BitArray('')]
	for itemlist in itemlists_by_bitcount:
		# assign codewords to items of n bits
		if len(itemlist) > len(_prefixes):
			return False
		for item in itemlist:
			dict_codes[item] = Bits(_prefixes.pop())		
		# add one bit to codewords
		###SCHEME 1
		#l = len(_prefixes)
		#_prefixes += [_prefixes[i][:] for i in range(l)]
		#for i in range(0,l):
		#	_prefixes[i].append('0b1')
		#for i in range(l,2*l):
		#	_prefixes[i].append('0b0')
		###SCHEME 2 (~ 12% slower)
		for i in range(0,2*len(_prefixes),2):
			_prefixes.insert(i+1,_prefixes[i][:])
			_prefixes[i].append('0b1')
			_prefixes[i+1].append('0b0')
	# End loop
	return True
# End function


# Arrange items by their limit of bits
#	-> dict = input[bitlimit] == no. of items w/ bit limit
def code_exists_for_itemcounts_per_bit(itemcounts_by_bitcount):
	no_of_prefixes = 1
	for no_of_items in itemcounts_by_bitcount:
		# assign codewords to items in sub-range
		if no_of_items > no_of_prefixes:
			return False
		no_of_prefixes-=no_of_items
		# add one bit to codewords
		no_of_prefixes*=2
	# End loop
	return True
# End function

# Build bitlimit array for use in coding functions
def itemlists_by_bitcounts_from_bar(bar, max_bits):
	return [range(	int(ceil(f_x_min_worst(b,bar))), \
					int(ceil(f_x_min_worst(b+1,bar)))	)\
		if b>0 else [] for b in range(max_bits+1)]

def itemlists_by_bitcount_lexibinary(max_bits):
	result=[]
	for i in range(1,max_bits,2):
		

def itemcounts_by_bitcount_from_bar(bar, max_bits):
	return [int(ceil(f_x_min_worst(b+1,bar))-ceil(f_x_min_worst(b,bar)))\
		if b>0 else 0 for b in range(max_bits+1)]

# bwi = {2:[1,2,3], 4:[4,5]}
# d = {}
# code_for_itemlists_per_bit(bwi,d)

itemcount_by_bitcount_scheme1 = lambda b: \
	2**((b-3)/2) if b > 5 else (0,1,0,1,2,4)[b]

itemcount_by_bitcount_scheme2 = lambda b: \
	2**((b/2)-1) if b >= 3 else (0,1,0)[b]

def lowest_bar_inwhich_code_exists():
	# Variables
	bar = 1.0
	e = 1.0
	code_exists = True
	limits=[]
	
	x1=0
	x2=0
	while e > 0:
		e*=0.5
		bar+=(-e if code_exists else e)
		# generate bit limits
		limits = itemcounts_by_bitcount_from_bar(bar,NO_OF_BITS)
		# determine if code exists for bit limits
		code_exists = code_exists_for_itemcounts_per_bit(limits)
	if not code_exists:
		bar+=e
	return bar

# lowest bar = 0.5623413251903491
#			= 10**-0.25 (for x=3, b=4)
'''
def points_of_optimal_precision():
	bar = lowest_bar_inwhich_code_exists()
	itemcounts_by_bitlimit = itemcounts_by_bitcount_from_bar(bar,NO_OF_BITS)
	sum = 0
	for i in range(len(itemcounts_by_bitlimit)):
		if not itemcounts_by_bitlimit[i]==0:
			
		

'''


def find_bit_length_for_hybrid_code(cont_block_size):
	itemcounts = itemcounts_by_bitcount_from_bar(BAR, NO_OF_BITS)
	itemcounts[5]+=1 # adds rep. for '0'
	
	no_of_prefixes = 1
	for i in range(NO_OF_BITS-cont_block_size):
		# assign codewords to items in sub-range
		if itemcounts[i] > no_of_prefixes:
			return False
		no_of_prefixes-=itemcounts[i]
		
		# test hybrid coding, starting from i+1
		debt=-no_of_prefixes+1-itemcounts[i]
		passed=True
		reps=0
		for j in range(i,NO_OF_BITS, cont_block_size+1):
			debt+=sum(itemcounts[j:j+cont_block_size+1])
			if 2**(cont_block_size*reps) <= debt:
				passed=False
				break
			reps+=1
		if passed:
			return i+1
		# add one bit to make codewords that are available for next loop
		no_of_prefixes*=2
	# End loop
	return True
# End function

def no_of_available_prefixes(itemcounts_per_bit):
	nop = [1]
	for ic in itemcounts_per_bit[1:]:
		nop.append(nop[-1]*2 - ic)
	return nop

def optimize_itemcounts_per_bitcount(itemcounts_by_bitcount):
	for i in range(1,len(itemcounts_by_bitcount)):
		while itemcounts_by_bitcount[i] > 0:
			itemcounts_by_bitcount[i]  -=1
			itemcounts_by_bitcount[i-1]+=1
			if not code_exists_for_itemcounts_per_bit(itemcounts_by_bitcount):
				itemcounts_by_bitcount[i]  +=1
				itemcounts_by_bitcount[i-1]-=1
				break
# End Function


def plot_increase_factor_of_itemcounts():
	ic = itemcounts_by_bitcount_from_bar(BAR, NO_OF_BITS)
	ic[5]+=1 # add code for '0': '11000'
	
	sum = ic[1]
	factors = []
	for i in range(2,len(ic)):
		factors.append((sum+ic[i])/float(sum))
		sum+=ic[i]
	
	# Plot
	plt.plot(range(2,len(ic)), factors,  'b-')
	plt.show()

def plot_increase_factor_of_itemcounts2():
	ic = itemcounts_by_bitcount_from_bar(BAR, NO_OF_BITS)
	ic[5]+=1 # add code for '0': '11000'
	
	factors = [ic[i]/float(ic[i-1]) for i in range(4,len(ic))]
	
	# Plot
	plt.plot(range(4,len(ic)), factors,  'b-')
	plt.show()


def get_possible_coderates_for_itemcounts_by_bitcounts(itemcounts):
	extracounts = no_of_available_prefixes(itemcounts)
	itemcount_factors = [ float(itemcounts[i+1])/itemcounts[i] \
			if not itemcounts[i] is 0 else float('inf') for i in range(len(itemcounts)-1)]
	itemcount_factors.append(1)
	max_factors = [2-(float(itemcounts[i])/extracounts[i]) \
			if not extracounts[i] is 0 else 1 for i in range(len(itemcounts))]
	
	# Plot
	plt.plot(range(len(itemcounts)), itemcount_factors,  'r-')
	plt.plot(range(len(itemcounts)), max_factors,  'b-')
	plt.show()

