################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/galaxy.cpp \
../src/graphics.cpp \
../src/language.cpp \
../src/main.cpp \
../src/object.cpp \
../src/shoot.cpp \
../src/sound.cpp \
../src/vector_2.cpp 

OBJS += \
./src/galaxy.o \
./src/graphics.o \
./src/language.o \
./src/main.o \
./src/object.o \
./src/shoot.o \
./src/sound.o \
./src/vector_2.o 

CPP_DEPS += \
./src/galaxy.d \
./src/graphics.d \
./src/language.d \
./src/main.d \
./src/object.d \
./src/shoot.d \
./src/sound.d \
./src/vector_2.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/Library/Frameworks/SDL.framework/Versions/A/Headers -I/Library/Frameworks/SDL_image.framework/Versions/A/Headers -O0 -arch i386 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


