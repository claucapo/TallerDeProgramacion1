Notas:

- Las funciones aceptarCliente, validarCliente y enviarMapa podrían ser bloqueantes en cuanto al
acceso a los datos de la partida. Lo ideal sería crear el cliente y utilizar el método .start()
para que comience con la recepción de updates en el mismo momento en que se toma una instantánea
del escenario para mandar.
Esta parte puede traer problemas de sincronismo, se puede solucionar haciendo que los jugadores
tengan que esperar a que se envíe completamente el mapa a un nuevo jugador. La otra es encontrar
una manera inteligente de utilizar los bloqueos.