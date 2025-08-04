#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor{
public:
	virtual ~Processor() = default;
	
	virtual void process() = 0;
	
}; // Processor

#endif // PROCESSOR_H
