import random
import math

letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'

def populate_identity_matrix(number_nodes):
   temp_matrix = [[0 for x in range(number_nodes)] for x in range(number_nodes)]
   for x in range(number_nodes):
      for y in range(number_nodes):
         if x < y:
            temp_matrix[x][y] = int(round(random.random() * 1))
         elif x == y:
            temp_matrix[x][y] = 2
         else:
            temp_matrix[x][y] = temp_matrix[y][x]
   return temp_matrix

def print_identity_matrix(matrix):
   for x in range(len(matrix)):
      currentRow = ""
      for y in range(len(matrix[x])):
         currentRow += str(matrix[x][y]) + " "
      print currentRow

def populate_graph(matrix):
   temp_graph = dict()
   for x in range(len(matrix)):
      temp_graph[letters[x]] = set()
      for y in range(len(matrix[x])):
         if matrix[x][y] == 1:
            temp_graph[letters[x]].add(letters[y])
   return temp_graph

def depth_first_search(graph, start, goal):
   stack = [(start, [start])]
   while stack:
      (vertex, path) = stack.pop()
      for next in graph[vertex] - set(path):
         if next == goal:
            yield path + [next]
         else:
            stack.append((next, path + [next]))

def add_connections(graph, matrix):
   start_node = 'A'
   start_index = letters.index(start_node)
   for x in range(len(graph) - 1):
      current_node = letters[x+1]
      current_index = x + 1
      if list(depth_first_search(graph, start_node, current_node)):
         pass
      else:
         print start_node + ' is added to ' + current_node
         print current_node + ' is added to ' + start_node

         graph[start_node].add(current_node)
         graph[current_node].add(start_node)

         matrix[start_index][current_index] = 1
         matrix[current_index][start_index] = 1
   return graph, matrix

def populate_overlay_network(total):
   temp_overlay = dict()
   valid_nodes = letters[:total]
   for x in valid_nodes:
      temp_overlay[x] = dict()
   while(valid_nodes):
      currentNode = valid_nodes[int(random.random() * len(valid_nodes))]
      temp_overlay[currentNode]['overlay_node'] = total - len(valid_nodes)
      valid_nodes = valid_nodes.replace(currentNode, "")
   return temp_overlay

def print_overlay_network(overlay_network):
   for node in overlay_network:
      print node + ' ' + str(overlay_network[node]['overlay_node'])

def create_finger_table_entry(list_supernode, number_entry, current_node, num_nodes):
   temp_finger_table = dict()
   for x in range(number_entry):
      temp_finger_table_entry = current_node + (2 ** x)
      if temp_finger_table_entry > num_nodes:
         temp_finger_table_entry = temp_finger_table_entry % num_nodes
      if temp_finger_table_entry > list_supernode[len(list_supernode)-1] or temp_finger_table_entry <= list_supernode[0]:
         temp_finger_table[x + 1] = list_supernode[0]
      else:
         for supernode in list_supernode:
            if temp_finger_table_entry <= supernode:
               temp_finger_table[x + 1] = supernode
               break
   return temp_finger_table

def find_key(start, key, overlay, found = False):
   print 'At node ' + str(start) + ':'
   current_finger_table = overlay[letters[start]]['finger_table']
   current_keys = overlay[letters[start]]['keys']
   print 'Finger Table: '
   print '%5s %5s'%('entry','node')
   for entry in current_finger_table.keys():
      print '%5s %5s'%(str(entry),str(current_finger_table[entry]))
   string_keys = ''
   for key_number in range(len(current_keys)):
      string_keys += str(current_keys[key_number]) + ' '
   print 'Keys at node ' + str(start) + ': ' + string_keys

   if key in current_keys:
      print 'Found key ' + str(key) + ' at node ' + str(start)
      found = True
      return
   elif not found:
      if key < start:
         for entry in reversed(range(len(current_finger_table.keys()))):
            if current_finger_table[entry + 1] <= key:
               print 'Use table entry ' + str(entry + 1) + ' to go to node ' + str(current_finger_table[entry + 1]) + '.\n'
               find_key(current_finger_table[entry + 1], key, overlay)
               return
         print 'Use table entry ' + str(len(current_finger_table.keys())) + ' to go to node ' + str(current_finger_table[len(current_finger_table.keys())]) + '.\n'
         find_key(current_finger_table[1], key, overlay)
         return
      else:
         for entry in range(len(current_finger_table.keys())):
            if current_finger_table[entry + 1] >= start:
               print 'Use table entry ' + str(entry + 1) + ' to go to node ' + str(current_finger_table[entry + 1]) + '.\n'
               find_key(current_finger_table[entry + 1], key, overlay)
               return
         print 'Use table entry ' + str(len(current_finger_table.keys())) + ' to go to node ' + str(current_finger_table[len(current_finger_table.keys())]) + '.\n'
         find_key(current_finger_table[len(current_finger_table.keys())], key, overlay)
         return

def main():
   num_nodes = input("How many nodes do you want to simulate? ")

   identity_matrix = populate_identity_matrix(num_nodes)

   num_table_entry = int(math.sqrt(num_nodes))

   print 'Here is the randomized identity matrix.'
   print '0 : Represents no connection between nodes.'
   print '1 : Represents a connection to another node.'
   print '2 : Represents a connection to self.\n'

   print_identity_matrix(identity_matrix)
   graph = populate_graph(identity_matrix)

   print '\nNow we add connections to node A to guarentee connections with all nodes.'

   graph, identity_matrix = add_connections(graph, identity_matrix)

   print '\nNow we show the connections between the nodes in the identity matrix.\n'
   print_identity_matrix(identity_matrix)

   print '\nWe now have the physical layout of network, next up is to put it in an overlay network.'
   overlay_network = populate_overlay_network(num_nodes)

   print '\nThis is the overlay network node number of each node.\n'
   print_overlay_network(overlay_network)

   num_supernodes = input("\nHow many supernodes do you want to simluate? ")

   list_supernode = list()

   for x in range(num_supernodes):
      temp_index = int(random.random() * num_nodes)
      while temp_index in list_supernode:
         temp_index = int(random.random() * num_nodes)
      list_supernode.append(temp_index)
   print "\n1 represents supernode, 0 represents non-supernode.\n"
   for x in range(num_nodes):
      if x in list_supernode:
         overlay_network[letters[x]]['supernode'] = 1
      else:
         overlay_network[letters[x]]['supernode'] = 0
      print str(x) + ':' + str(overlay_network[letters[x]]['supernode'])

   print '\nWe need int(sqrt(' + str(num_nodes) + ")): " + str(num_table_entry) + " table entries.\n"

   list_supernode.sort()
   print list_supernode
   print ''

   print 'Super node is responsible for these entries.\n'
   for x in range(len(list_supernode)):
      print str(list_supernode[x]) + ':'
      overlay_network[letters[list_supernode[x]]]['keys'] = list()
      if x == 0:
         for node in range(num_nodes):
            if node > list_supernode[len(list_supernode) - 1] or node <= list_supernode[0]:
               overlay_network[letters[list_supernode[x]]]['keys'].append(node)
      else:
         for node in range(num_nodes):
            if node <= list_supernode[x] and node > list_supernode[x - 1]:
               overlay_network[letters[list_supernode[x]]]['keys'].append(node)
      print overlay_network[letters[list_supernode[x]]]['keys']
      print ''

   for x in range(len(list_supernode)):
      overlay_network[letters[list_supernode[x]]]['finger_table'] = dict()
      overlay_network[letters[list_supernode[x]]]['finger_table'] = create_finger_table_entry(list_supernode, num_table_entry, list_supernode[x], num_nodes)
      print overlay_network[letters[list_supernode[x]]]['finger_table']

   print '\nHere are the supernodes:'
   print list_supernode

   while True:
      start_node = input('\nWhich supernode would you like to start with? ')
      if start_node in list_supernode:
         break

   while True:
      key_number = input('What is the key you are looking for? ')
      if key_number in range(num_nodes):
         break

   print ''

   find_key(start_node, key_number, overlay_network)

if __name__ == '__main__':
   main()
