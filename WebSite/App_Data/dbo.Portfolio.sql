CREATE TABLE [dbo].[Portfolio] (
    [Id]       INT IDENTITY (1, 1) NOT NULL,
    [modele_id] INT NULL,
    PRIMARY KEY CLUSTERED ([Id] ASC),
    CONSTRAINT [FK_Portfolio_ToModele] FOREIGN KEY ([modele_id]) REFERENCES [dbo].[Modele] ([Id])
);