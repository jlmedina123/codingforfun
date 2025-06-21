
import matplotlib.pyplot as plt
import numpy as np
import os
import tensorflow as tf
import tensorflow.keras.layers as tfl

from tensorflow.keras.preprocessing import image_dataset_from_directory
from tensorflow.keras.layers.experimental.preprocessing import RandomFlip, RandomRotation

AUTOTUNE = tf.data.experimental.AUTOTUNE
train_dataset = train_dataset.prefetch(buffer_size=AUTOTUNE)


def data_augmenter():
    '''
    Create a Sequential model composed of 2 layers
    Returns:
        tf.keras.Sequential
    '''
    print("TensorFlow version:", tf.__version__)
    
    data_augmentation = tf.keras.models.Sequential()
    #data_augmentation.add(tf.keras.layers.RandomFlip("horizontal")) # 2.4 and above
    data_augmentation.add(tfl.experimental.preprocessing.RandomFlip("horizontal")) # 2.3
    #data_augmentation.add(tfl.Lambda(lambda x: tf.keras.preprocessing.image.random_flip(x))) #older
    
    #data_augmentation.add(tfl.RandomRotation(0.2))
    data_augmentation.add(tfl.experimental.preprocessing.RandomRotation(0.2))
    #data_augmentation.add(tfl.Lambda(lambda x: tf.keras.preprocessing.image.random_rotation(x)))
    
    return data_augmentation

data_augmentation = data_augmenter()

for image, _ in train_dataset.take(1):
    plt.figure(figsize=(10, 10))
    first_image = image[0]
    for i in range(9):
        ax = plt.subplot(3, 3, i + 1)
        augmented_image = data_augmentation(tf.expand_dims(first_image, 0))
        plt.imshow(augmented_image[0] / 255)
        plt.axis('off')


preprocess_input = tf.keras.applications.mobilenet_v2.preprocess_input

IMG_SHAPE = IMG_SIZE + (3,)
base_model = tf.keras.applications.MobileNetV2(input_shape=IMG_SHAPE,
                                               include_top=True,
                                               weights='imagenet')


def alpaca_model(image_shape=IMG_SIZE, data_augmentation=data_augmenter()):
    ''' Define a tf.keras model for binary classification out of the MobileNetV2 model
    Arguments:
        image_shape -- Image width and height
        data_augmentation -- data augmentation function
    Returns:
    Returns:
        tf.keras.model
    '''
    
    
    input_shape = image_shape + (3,)
    
    ### START CODE HERE
    
    base_model = tf.keras.applications.MobileNetV2(input_shape=IMG_SHAPE,
                                                   include_top=False,  # <== Important!!!!
                                                   weights='imagenet') # From imageNet
    
    # freeze the base model by making it non trainable
    base_model.trainable = False 

    # create the input layer (Same as the imageNetv2 input size)
    input_layer = tf.keras.Input(shape=input_shape, name='input_layer') 
    
    # apply data augmentation to the inputs
    augmented_input = data_augmentation(input_layer)
    
    # data preprocessing using the same weights the model was trained on (x = preprocess_input(x))
    preprocessed_input = tf.keras.applications.mobilenet_v2.preprocess_input(augmented_input)
    
    # set training to False to avoid keeping track of statistics in the batch norm layer
    output_base = base_model(preprocessed_input, training=False)
    
    # add the new Binary classification layers
    # use global avg pooling to summarize the info in each channel (x = None()(x))
    global_average_layer = tfl.GlobalAveragePooling2D()(output_base)

    # include dropout with probability of 0.2 to avoid overfitting (x = None(None)(x))
    dropout_layer = tfl.Dropout(0.2)(global_average_layer)

    # use a prediction layer with one neuron (as a binary classifier only needs one)
    dense_layer = tfl.Dense(1, activation='linear')(dropout_layer)
    
    model = tf.keras.Model(inputs=input_layer, outputs=dense_layer)
    
    return model

base_model = model2.layers[4]
base_model.trainable = True
# Let's take a look to see how many layers are in the base model
print("Number of layers in the base model: ", len(base_model.layers))

# Fine-tune from this layer onwards
fine_tune_at = 120

# Freeze all the layers before the `fine_tune_at` layer
for layer in base_model.layers[:fine_tune_at]:
    layer.trainable = False
    
# Define a BinaryCrossentropy loss function. Use from_logits=True
loss_function = tf.keras.losses.BinaryCrossentropy(from_logits=True)
# Define an Adam optimizer with a learning rate of 0.1 * base_learning_rate
optimizer = tf.keras.optimizers.Adam(learning_rate= 0.1 * base_learning_rate)
# Use accuracy as evaluation metric
metrics=['accuracy']

model2.compile(loss=loss_function,
              optimizer = optimizer,
              metrics=metrics)
