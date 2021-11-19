# artificial intelligence

https://en.wikipedia.org/wiki/Artificial_intelligence

goals: reasoning, knowledge representation, planning, learning, natural language processing, perception, and the ability to move and manipulate objects
techniques: search and mathematical optimization, formal logic, artificial neural networks, statistics, probability and economics

# machine learning

algorithms that can improve automatically through experience and by the use of data

algorithms build a model based on sample data, known as training data, in order to make predictions or decisions

to teach computers to accomplish tasks where no fully satisfactory algorithm is available

objectives
 - classify data
 - make predictions for future outcomes

 machine learning focuses on prediction, based on known properties learned from the training data -> goal: reproduce known knowledge
 data mining focuses on the discovery of (previously) unknown properties in the data -> goal: discovery of previously unknown knowledge

Generalization: ability of a learning machine to perform accurately on new, unseen examples/tasks after having experienced a learning data set

ML division
 - Supervised learning: The computer is presented with example inputs and their desired outputs
 - Unsupervised learning: No labels are given to the learning algorithm, leaving it on its own to find structure in its input
 - Reinforcement learning: A computer program interacts with a dynamic environment in which it must perform a certain goal

AI applications
 - classifiers: use pattern matching. Trained by ML algorithms:
   - decision tree
   - Bayes classifier
   - Neural network
 - controllers

# Neural Networks

https://en.wikipedia.org/wiki/Artificial_neural_network

Neuron: receives signal, processes it (non-linear function of sum of inputs), transmit
Signal: real number
Edges: connections
Weights: incr/decr strength of signal at connection
Threshold: under which signal not sent
Layers: perform different transformations

input -> result
error = network output (prediction) - target output

Models
 - node: neuron
   - output: weigthed sum of inputs (activation) + bias -> activation function -> output
 - link: connection, assigned a weight (importance)
   - propagation function: computes input to neuron from predecessor's outputs
 - weight: strength of link influence
 - layers:
   - types: input, hidden, output
   - connection types:
      - fully connected: every neuron one layer connects to every neuron next layer
      - pooling: group of neurons one layer connects to one neouron next layer -> directed acyclic graph -> feedforward networks
      - connection between neurons same or previous layers -> recurrent networks
 - hyperparameter: const parameter set before learning
 - learning:
    - adjust weights and thresholds to minimize error. complete when additional data does not change error rate
    - loss/cost function. opposite to objective function. must decrease during training/learning
    - learning rate: size of corrective steps to adjust for errors. highers -> less training time, less accuracy
       - adaptive learning rate: to avoid oscillation
    - backpropagation: 
       - adjust connection weights to compensate for errors during learning
       - error amount divided among connections
       - gradient of cost function for a state respect to weights
       - weight update via stochastic gradient descent 
    - types
       - supervised:  cost function = mean-squared error
       - unsupervised: eg cost function for compression x-f(x)
       - reinforcement: weight network to perform action that minimize long-term cost
           - Environment modeled as MDP
           - coupled with dynamic programming


https://en.wikibooks.org/wiki/Artificial_Neural_Networks

2. Neural Network Basics
  
for tasks with no known algorithm. when abundant data, but little underlying theory
Processing Elements = neurons = nodes
PEs defined as
 - weighted sum of inputs
 - function of weighted sum -> transfer function or activation function (if threshold comparison)

Static network: learning and production phases separate
Dynamic network: continue learning during production

overtraining: too many training examples, so no generalization

parameters: layers, neurons/layer, training iterations, learning rate, momentum

learning rate: size of weight and bias changes during learning
momentum: adds fraction m of previous weigth update to current weight update. Prevents converging to saddle point.
epoch: max number iterations
min error: root square of sum of squqred differeneces between target and output

perceptron:
  activation function = step function with (constant threshold)
  original perceptron one layer -> only linear problems
  Multi Layer Perceptrons -> non linear problems
  backpropagation: gradient descent algorith for minimization and curve-fitting


6. Activation Functions

 inputs x-n -> weights w-nj -> transfer function -> net input net-j , threshold z-j -> activation function -> activation o-j

activation functions 
- step function :     weigthed input sum above or below threshold -> for binary classification
- liner combination : weigthed input sum + bias
- continuous log-sigmoid
- contiouous tan-sigmoid 
- softmax : posterior probability

types of networks
- feedforward networks
    no feedback: w-ij = 0 if layer-i < layer-j
- RBF
    three layers
    activation function = gaussian kernel
- recurrent network
    weigth matrix layer l has input weights from all other neurons
- echo state networks
    recurrent networks, not all hidden layer neuron is connected to all input neurons


