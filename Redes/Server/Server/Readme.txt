Notas:

- Las funciones aceptarCliente, validarCliente y enviarMapa podr�an ser bloqueantes en cuanto al
acceso a los datos de la partida. Lo ideal ser�a crear el cliente y utilizar el m�todo .start()
para que comience con la recepci�n de updates en el mismo momento en que se toma una instant�nea
del escenario para mandar.
Esta parte puede traer problemas de sincronismo, se puede solucionar haciendo que los jugadores
tengan que esperar a que se env�e completamente el mapa a un nuevo jugador. La otra es encontrar
una manera inteligente de utilizar los bloqueos.