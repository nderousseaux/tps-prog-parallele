#On ouvre les données
data = read.csv2('./ex1.result', dec=".")


## --------BASE-------- ##
b_data = subset(data, data$TYPE == 0)
b_time_moyen = aggregate(
          x= b_data$TIME,
          by = list(b_data$NBTHREAD),      
          FUN = mean)
nbProc = b_time_moyen$Group.1
b_temps = b_time_moyen$x
b_speed_up = b_temps[1]/b_temps
b_effi = b_speed_up/nbProc

## --------STATIC-------- ##
s_data = subset(data, data$TYPE == 1)
s_time_moyen = aggregate(
  x= s_data$TIME,
  by = list(s_data$NBTHREAD),      
  FUN = mean)
s_temps = s_time_moyen$x
s_speed_up = s_temps[1]/s_temps
s_effi = s_speed_up/nbProc

## --------DYNAMIC-------- ##
d_data = subset(data, data$TYPE == 2)
d_time_moyen = aggregate(
  x= d_data$TIME,
  by = list(d_data$NBTHREAD),      
  FUN = mean)
d_temps = d_time_moyen$x
d_speed_up = d_time_moyen$x[1]/d_time_moyen$x
d_effi = d_speed_up/nbProc


## --------TEMPS-------- ##
plot(
  main="tmpExec/nbThreads",
  xlab="nbThreads",
  ylab="tmpExec",
  xlim=c(min(nbProc),max(nbProc)), 
  ylim=c(0,max(c(b_temps, d_temps, s_temps))), 
  x= nbProc,
  y= b_temps,
  type='o',
  col="blue"
)
points(
  x= nbProc,
  y= s_temps,
  type="o",
  col="red"
)
points(
  x= nbProc,
  y= d_temps,
  type="o",
  col="green"
)
legend("bottomleft", 
       legend = c("Base", "Static", "Dynamic"), 
       col = c("blue","red", "green"), 
       lty=1)

## --------SPEEDUP-------- ##
plot(
  main="speedUp/nbThreads",
  xlab="nbThreads",
  ylab="speedup",
  xlim=c(min(nbProc),max(nbProc)), 
  ylim=c(0,max(c(b_speed_up, s_speed_up, d_speed_up))), 
  x= nbProc,
  y= b_speed_up,
  type='o',
  col="blue"
)
points(
  x= nbProc,
  y= s_speed_up,
  type="o",
  col="red"
)
points(
  x= nbProc,
  y= d_speed_up,
  type="o",
  col="green"
)
legend("bottomleft", 
       legend = c("Base", "Static", "Dynamic"), 
       col = c("blue","red", "green"), 
       lty=1)


## --------EFFICIENCY-------- ##
plot(
  main="efficiency/nbThreads",
  xlab="nbThreads",
  ylab="efficiency",
  xlim=c(min(nbProc),max(nbProc)), 
  ylim=c(0,max(c(b_effi, d_effi, s_effi))), 
  x= nbProc,
  y= b_effi,
  type='o',
  col="blue"
)
points(
  x= nbProc,
  y= s_effi,
  type="o",
  col="red"
)
points(
  x= nbProc,
  y= d_effi,
  type="o",
  col="green"
)
legend("bottomleft", 
       legend = c("Base", "Static", "Dynamic"), 
       col = c("blue","red", "green"), 
       lty=1)

