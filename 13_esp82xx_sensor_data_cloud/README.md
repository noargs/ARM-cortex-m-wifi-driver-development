We used [ThingSpeak](https://thingspeak.com/) in this project, Which allows us to send our data to cloud and provide realtime analytics in the dashboard.    
    
You need [MathWorks](https://mathworks.com/) account to access ThingSpeak.   
     
You can have multiple [Channels](https://thingspeak.com/channels) to receive data from different projects.      
     
Create new channel by providing the **Name** i.e. _ESP Single Sensor_, **Field 1** as _PA1 Sensor_, leave everything else blank and hit _Save Channel_   
    
Our created channel look something like below image. Next click on **API Keys** to get API Keys for _Write_ and _Read_ from the channel.       
		
<img src="images/created_channel.png" alt="Channel window" title="Channel window">    
     
`espxx_thingspeak_send()` API uses AT command `AT+CIPSTART` in `esp82xx_lib.c` to create TCP connection and communicate with ThingSpeak via GET request   
     
<img src="images/at_cipstart.png" alt="ESP8266 AT Instructions" title="ESP8266 AT Instructions">            
     
You can make following _API Requests_    
     
**Write a Channel Feed**   
`GET https://api.thingspeak.com/update?api_key=<YOUR_WRITE_API_KEY_HERE>&field1=0`      
     
**Read a Channel Feed**   
`GET https://api.thingspeak.com/channels/<YOUR_CHANNEL_ID>/feeds.json?api_key=<YOUR_READ_API_KEY_HERE>&results=2`        
     
**Read a Channel Feed**   
`GET https://api.thingspeak.com/channels/<YOUR_CHANNEL_ID>/fields/1.json?api_key=<YOUR_READ_API_KEY_HERE>&results=2`    
     
**Read Channel Status Updates**   
`GET https://api.thingspeak.com/channels/<YOUR_CHANNEL_ID>/status.json?api_key=<YOUR_READ_API_KEY_HERE>`           
            
        
          
     
        
		  	 			  	 		
    		 	 			 					  	 		
    		 	 			
    	 	 				  
    		 	 			
    
		
		
    



			
	 		 

         
		 
           
		 
     
		  	 						 		 
		     
		 
	
    
    
    
    
    
    
    
    
    
  
    
    
    
    
    
    
    
    

     
     

     
     

     
    
    