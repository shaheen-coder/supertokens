from shatokens_api import BPE

from model import Model


class BpeTrainer(BPE):

    def __init__(self, vocab_size : int ) -> None :

        super().__init__(vocab_size)


    def save(self, filename : str ) -> None:

        Model(filename, [
            self.vocab_size, # int 
            self.vocab, # list [ int ]
            self.merge_rank, # dict [ int , int ]
            self.token_to_id # dict [str , int ]
            
        ]).save()

    def load(self,filename : str) -> None :

        weigths : list = Model(filename,[]).load()

        for weight in weigths:

            if isinstance(weight, int) :
                self.vocab_size = weight
            if isinstance(weight, list):
                self.vocab = weight
            if( isinstance(weight, dict)):
                if (
                    all(isinstance(k, int) for k in weight.keys())
                    and
                    all(isinstance(v, int) for v in weight.values())
                ):
                    self.merge_rank = weight

                elif (
                    all(isinstance(k, str) for k in weight.keys())
                    and
                    all(isinstance(v, int) for v in weight.values())
                ):
                    self.token_to_id = weight
