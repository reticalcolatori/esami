#!/bin/sh

# if (System.getSecurityManager() == null){
# 				System.setProperty("java.security.policy", "rmi.policy");
# 				System.setSecurityManager(new RMISecurityManager());
# 			}

rmiregistry -J-Djava.security.policy=rmi.policy
java RMI_Server
java RMI_Client localhost 