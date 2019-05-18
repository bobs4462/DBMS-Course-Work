CREATE UNIQUE INDEX idx_appointment_tr ON appointment(tabid, recdatetime); 
CREATE UNIQUE INDEX idx_appointment_rr ON appointment(regid, recdatetime);
