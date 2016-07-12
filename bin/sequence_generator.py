#!/usr/bin/python

import sys
import random

class Level:
	def __init__(self):
		self.orders = []
	def __str__(self):
		level = ""
		for order in self.orders:
			level += ("%d,"%(order['id']))
		return level.replace("\n", ",")

class OrderBookSide:
	def __init__(self, side, order_book):
		self.side = side
		self.levels = {}
		self.order_book = order_book

	def add_order(self, order):
		new_order = {'id':order['id'], 'qty':order['qty'], 'price':order['price']}
		order_price = order['price']
		if self.levels.has_key(order_price) == False:
			self.levels[order_price] = Level()
		self.levels[order_price].orders.append(new_order)
		self.order_book.stats[self.side].depth[order_price] += 1
		self.order_book.stats[self.side].total_qty += order['qty']

	def get_intersection(self, price):
		print sorted(self.levels.keys())
		if self.side == 'S':
			return [key for key in sorted(self.levels.keys()) if price >= key]
		return [key for key in sorted(self.levels.keys(), reverse = True) if price <= key]

class Statistics:
	def __init__(self):
		self.depth = {}
		self.total_qty = 0
	def __str__(self):
		length =  len([v for v in self.depth.values() if v > 0])
		print "Length:", [k for k in self.depth.items() ]
		print "Length:", [v for v in self.depth.values() if v > 0]
		return 'depth %d, total qty %d'%(length, self.total_qty)

class OrderBook:
	def __init__(self, depth):
		self.bid = OrderBookSide('B', self)
		self.ask = OrderBookSide('S', self)
		self.sides = {'B':self.bid, 'S':self.ask}
		self.action = ['A','X','M','T']
		self.price_step = random.randrange(1, 10, 1)
		self.min_price = random.randrange(1, 10000, 1)
		self.max_price = self.min_price + depth*2*self.price_step
		self.prices = [price for price in range(self.min_price, self.max_price, self.price_step)]
		self.mid_price = self.prices[depth - 1]
		self.orders = {}
		self.next_order_id = 1
		self.stats = {'B': Statistics(), 'S': Statistics()}
		self.trades = 0
		for prc in self.prices:
			print prc
			self.stats['S'].depth[prc] = 0
			self.stats['B'].depth[prc] = 0
		print self.prices

	def new_order(self):
		qty = random.randrange(1, 100, 1)
		order_id = self.get_order_id()
		price = random.choice(self.prices)

		side = 'S' if price > self.mid_price else 'B'

		order = {'action':'A', "id":order_id, "side":side, "qty":qty, "price":price}
		self.sides[side].add_order(order)
		self.orders[order_id] = order
		return order

	def build_initial_order_book(self):
		line = str()
		for i in range(0, 10, 1):
			order = self.new_order()

			line += "%s,%d,%s,%d,%d\n"%(order['action'], order['id'], order['side'], order['qty'], order['price'])
		return line

	def check_and_match(self, order):
		order_id = order['id']
		side = self.bid if order['side'] == 'B' else self.ask
		opposite_side = self.bid if order['side'] == 'S' else self.ask
		qty = order['qty']
		price = order['price']

		results = ""

		intersection = opposite_side.get_intersection(price)
		print "on the on ther side were found:",intersection

		if intersection:
			# Looks like we've got trades here
			for prc in intersection:
				if qty == 0:
					break
				level = opposite_side.levels[prc]
				print "%d: %s"%(prc, str(level))
				
				new_list_of_orders = []
				for victim in level.orders:
					print "Victim order is:", victim
					if qty == 0:
						new_list_of_orders.append(victim)
					else:
						if victim['qty'] <= qty: # removing the order
							results += "X,%d,%s,%d,%d\n"%(victim['id'], opposite_side.side, victim['qty'], victim['price'])
							results += "T,%d,%d\n"%(victim['qty'],prc)
							qty -= victim['qty']
							self.stats[opposite_side.side].total_qty -= victim['qty']
							self.stats[opposite_side.side].depth[prc] -= 1
							self.trades += 1

							print "removing victim:", victim, "depth:", self.stats[opposite_side.side].depth[prc]
							print "need to trade the rest", qty
						else:
							self.stats[opposite_side.side].total_qty -= qty
							victim['qty'] -= qty

							results += "T,%d,%d\n"%(qty,prc)
							results += "M,%d,%s,%d,%d\n"%(victim['id'], opposite_side.side, victim['qty'], victim['price'])
							self.trades += 1
							
							new_list_of_orders.append(victim)
							print "updating victim:", victim
							print results
							qty = 0

				level.orders = new_list_of_orders
				if len(new_list_of_orders) == 0:
					print "trying to remove %d price from side %s"%(prc, opposite_side.side)
					del opposite_side.levels[prc]
			if qty > 0:
				print "-------------removing order to another side:",side.side,"->",opposite_side.side
				rest_order = {'action':'A','id':self.get_order_id(),'side':opposite_side,'qty':qty,'price':price}
				self.sides[opposite_side.side].add_order(rest_order)

				results += "X,%d,%s,%d,%d\n"%(order_id, side.side, order['qty'], price)
				results += "A,%d,%s,%d,%d\n"%(rest_order['id'], opposite_side.side, qty, price)

			elif qty == 0:
				results += "X,%d,%s,%d,%d\n"%(order_id, side.side, order['qty'], price)

				
		else:
			print "No match with opposite side, simply adding"
			self.sides[side.side].add_order(order)

			pass
		return results


	def get_next_event(self):
		line = str()
		side = self.sides[random.choice(['B','S'])]
		action = random.choice(self.action)
		action = 'A'
		if action == 'A':
			qty = random.randrange(1, 100, 1)
			order_id = self.get_order_id()
			order = {'action':'A','id':order_id,'side':side.side,'qty':qty,'price':random.choice(self.prices)}

			line += "%s,%d,%s,%d,%d\n"%(order['action'], order['id'], order['side'], order['qty'], order['price'])
			print "Generated order: ",line
			line += self.check_and_match(order)
		else:
			pass

		return line

	def get_order_id(self):
		ret = self.next_order_id
		self.next_order_id += 1
		return ret


def generate():
	seed = "1"
	random.seed(seed)

	if len(sys.argv) == 1:
		result = "sequence-" + seed + ".txt"
		summary = "sequence-summary-" + seed + ".txt"
	else:
		result = sys.argv[1] + ".txt"
		summary = sys.argv[1] + "-summary.txt"


	order_book = OrderBook(20)

	with open(result, "w+") as orders:
		initital_orders = order_book.build_initial_order_book()
		orders.write(initital_orders)
		print initital_orders

		for i in range(1000000):
			print "\n------------new iteration"
			orders.write(order_book.get_next_event())

	with open(summary, "w+") as order_book_summmary:
		order_book_summmary.write('Buy: %s\n'%(order_book.stats['B']))
		order_book_summmary.write('Sell: %s\n'%(order_book.stats['S']))
		order_book_summmary.write('Trades: %d\n'%(order_book.trades))
	print "--------------------"

if __name__ == "__main__":
	if len(sys.argv) > 1 and sys.argv[1] == "-h":
		print "./sequence_generator.py <seed>"
		sys.exit(0)
	generate()
